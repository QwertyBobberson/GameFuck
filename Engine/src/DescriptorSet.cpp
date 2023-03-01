#include "../include/DescriptorSet.hpp"

DescriptorSet::DescriptorSet(std::vector<Descriptor> descriptors)
{
    this->descriptors = descriptors;
}