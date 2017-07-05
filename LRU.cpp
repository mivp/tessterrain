#include "LRU.h"

namespace tessterrain {

size_t LRUCache::prune() {
    if (m_maxSize > 0 && m_cache.size() >= (m_maxSize + m_elasticity)) {
        size_t count = 0;
        while (m_cache.size() > m_maxSize) {
            Node* n = m_keys.pop();
            n->value->unloadTextures();
            m_cache.erase(n->key);
            delete n;
            count++;
            std::cout << "LRU free data " << std::endl;
        }
        return count;
    } else {
        return 0;
    }
}

}; // namespace tessterrain
