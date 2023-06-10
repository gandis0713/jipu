#include "sample.h"

Sample::Sample(const SampleDescriptor& descriptor)
    : Window(descriptor.windowDescriptor)
    , m_path(descriptor.path)
{
}