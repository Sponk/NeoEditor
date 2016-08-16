#include "Publisher.h"

std::unordered_map<std::string, Publish::Publisher*> Publish::PublisherRegistry::m_publishers;
