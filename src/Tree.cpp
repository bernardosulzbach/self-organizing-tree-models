#include "Tree.hpp"
#include "BoundingBox.hpp"

#include <iostream>

static constexpr float PipeModelExponent = 2.0f;
static constexpr float PipeModelLeafValue = 1.0e-8f;

Tree::Tree(Environment &environment, Point seedlingPosition) : environment(environment) {
  const auto end = seedlingPosition.translate(0.0f, 1.0f * Environment::MetamerBaseLength, 0.0f);
  root = std::make_unique<Metamer>(environment, seedlingPosition, end);
}

U64 Tree::countMetamers() const {
  if (!root) {
    return 0;
  }
  return root->countMetamers();
}

BoundingBox Tree::getBoundingBox() const {
  return root->getBoundingBox();
}

void Tree::performGrowthIteration() {
  // 1. Calculate local environment of all tree buds.
  environment.markerSet.resetAllocations();
  allocateMarkers(root);
  // 2. Determine the fate of each bud (the extended Borchert-Honda model).
  propagateLightBasipetally(root);
  root->growthResource = Environment::BorchertHondaAlpha * root->light;
  propagateResourcesAcropetally(root);
  // 3. Append new shoots.
  performGrowthIteration(root);
  // 4. Shed branches (not implemented).
  // 5. Update internode width for all internodes.
  updateInternodeWidths(root);
  tropismGrowthDirectionWeight *= TropismGrowthDirectionWeightAttenuation;
}

void Tree::allocateMarkers(std::unique_ptr<Metamer> &metamer) {
  if (!metamer) {
    return;
  }
  const auto theta = Environment::PerceptionAngle;
  const auto r = Environment::PerceptionRadiusFactor * metamer->getLength();
  if (!metamer->axillary) {
    environment.markerSet.updateAllocatedInCone(metamer->axillaryId, metamer->end, metamer->axillaryDirection, theta, r);
  } else {
    allocateMarkers(metamer->axillary);
  }
  if (!metamer->terminal) {
    const auto direction = Vector(metamer->beginning, metamer->end);
    environment.markerSet.updateAllocatedInCone(metamer->terminalId, metamer->end, direction, theta, r);
  } else {
    allocateMarkers(metamer->terminal);
  }
}

void Tree::propagateLightBasipetally(std::unique_ptr<Metamer> &metamer) {
  if (!metamer) {
    return;
  }
  propagateLightBasipetally(metamer->axillary);
  propagateLightBasipetally(metamer->terminal);
  const auto theta = Environment::PerceptionAngle;
  const auto r = Environment::PerceptionRadiusFactor * metamer->getLength();
  metamer->light = 0.0f;
  if (!metamer->axillary) {
    const auto budId = metamer->axillaryId;
    metamer->axillaryLight = environment.markerSet.getAllocatedInCone(budId, metamer->end, metamer->axillaryDirection, theta, r).q;
  } else {
    metamer->axillaryLight = metamer->axillary->light;
  }
  if (!metamer->terminal) {
    const auto direction = Vector(metamer->beginning, metamer->end);
    const auto budId = metamer->terminalId;
    metamer->terminalLight = environment.markerSet.getAllocatedInCone(budId, metamer->end, direction, theta, r).q;
  } else {
    metamer->terminalLight = metamer->terminal->light;
  }
  metamer->light = metamer->axillaryLight + metamer->terminalLight;
}

void Tree::propagateResourcesAcropetally(std::unique_ptr<Metamer> &metamer) {
  if (!metamer) {
    return;
  }
  const auto qM = metamer->terminalLight;
  const auto qL = metamer->axillaryLight;
  // Dodge divisions by zero if these branches have not acquired any light.
  if (qM + qL == 0.0f) {
    return;
  }
  const auto v = metamer->growthResource;
  const auto lambda = Environment::BorchertHondaLambda;
  const auto denominator = lambda * qM + (1.0f - lambda) * qL;
  const auto vM = v * (lambda * qM) / denominator;
  const auto vL = v * ((1.0f - lambda) * qL) / denominator;
  if (metamer->axillary) {
    metamer->axillary->growthResource = vL;
    metamer->axillaryGrowthResource = 0.0f;
    propagateResourcesAcropetally(metamer->axillary);
  } else {
    metamer->axillaryGrowthResource = vL;
  }
  if (metamer->terminal) {
    metamer->terminal->growthResource = vM;
    metamer->terminalGrowthResource = 0.0f;
    propagateResourcesAcropetally(metamer->terminal);
  } else {
    metamer->terminalGrowthResource = vM;
  }
  metamer->growthResource = 0.0f;
}

void Tree::performGrowthIteration(std::unique_ptr<Metamer> &metamer) {
  if (!metamer) {
    return;
  }
  if (!metamer->axillary) {
    metamer->axillary = addNewShoot(metamer->axillaryId, metamer->getLength(), metamer->end, metamer->axillaryDirection, metamer->terminalGrowthResource);
  } else {
    performGrowthIteration(metamer->axillary);
  }
  if (!metamer->terminal) {
    const auto direction = Vector(metamer->beginning, metamer->end);
    metamer->terminal = addNewShoot(metamer->terminalId, metamer->getLength(), metamer->end, direction, metamer->terminalGrowthResource);
  } else {
    performGrowthIteration(metamer->terminal);
  }
}

std::unique_ptr<Metamer> Tree::addNewShoot(BudId budId, float supportingMetamerLength, Point origin, Vector direction, float resource) {
  const auto theta = Environment::PerceptionAngle;
  const auto r = Environment::PerceptionRadiusFactor * supportingMetamerLength;
  const auto spaceAnalysis = environment.markerSet.getAllocatedInCone(budId, origin, direction, theta, r);
  if (spaceAnalysis.q == 0.0f) {
    return nullptr;
  }
  if (std::floor(resource) == 0.0f) {
    return nullptr;
  }
  std::unique_ptr<Metamer> headMetamer;
  std::unique_ptr<Metamer> *nextMetamer = &headMetamer;
  auto metamerEnd = origin;
  auto metamerDirection = direction;
  const auto optimalGrowthDirection = spaceAnalysis.v.normalize();
  const auto tropismDirection = Vector(0.0f, 1.0f, 0.0f).normalize();
  const auto metamerLength = resource / static_cast<int>(std::floor(resource)) * Environment::MetamerBaseLength;
  for (auto metamers = static_cast<int>(std::floor(resource)); metamers > 0; metamers--) {
    metamerDirection = metamerDirection.add(optimalGrowthDirection.scale(Environment::OptimalGrowthDirectionWeight));
    metamerDirection = metamerDirection.add(tropismDirection.scale(tropismGrowthDirectionWeight));
    metamerDirection = metamerDirection.normalize();
    const auto metamerVector = metamerDirection.scale(metamerLength);
    const auto previousMetamerEnd = metamerEnd;
    metamerEnd = metamerEnd.translate(metamerVector.x, metamerVector.y, metamerVector.z);
    environment.markerSet.removeMarkersInSphere(metamerEnd, Environment::OccupancyRadiusFactor * metamerLength);
    *nextMetamer = std::make_unique<Metamer>(environment, previousMetamerEnd, metamerEnd);
    nextMetamer = &(*nextMetamer)->terminal;
  }
  return headMetamer;
}

void Tree::updateInternodeWidths(std::unique_ptr<Metamer> &metamer) {
  if (!metamer) {
    return;
  }
  auto total = PipeModelLeafValue;
  updateInternodeWidths(metamer->axillary);
  if (metamer->axillary) {
    total += std::pow(metamer->axillary->width, PipeModelExponent);
  }
  updateInternodeWidths(metamer->terminal);
  if (metamer->terminal) {
    total += std::pow(metamer->terminal->width, PipeModelExponent);
  }
  metamer->width = std::pow(total, 1.0f / PipeModelExponent);
}
