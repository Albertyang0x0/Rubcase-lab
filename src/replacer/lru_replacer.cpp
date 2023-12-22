#include "lru_replacer.h"

LRUReplacer::LRUReplacer(size_t num_pages) { max_size_ = num_pages; }

LRUReplacer::~LRUReplacer() = default;

/**
 * @brief 使用LRU策略删除一个victim frame，这个函数能得到frame_id
 * @param[out] frame_id id of frame that was removed, nullptr if no victim was found
 * @return true if a victim frame was found, false otherwise
 */

//LRUlist:最近被访问的页面位于列表的尾部，而最早被访问的页面位于列表的头部
//LRUhash:快速查找页面在 LRUlist 中的位置。键是页面的标识符（frame_id_t），值是页面在 LRUlist 中的位置信息。
bool LRUReplacer::Victim(frame_id_t *frame_id) {
    
    std::lock_guard<std::mutex> lock(latch_);

    // Todo:
    //  利用lru_replacer中的LRUlist_,LRUHash_实现LRU策略
    //  选择合适的frame指定为淘汰页面,赋值给*frame_id

    if (LRUlist_.empty()) {
        // LRUlist为空，没有victim frame
        return false;
    }
    
    *frame_id = LRUlist_.back();
    LRUlist_.pop_back();
    LRUhash_.erase(*frame_id);

    return true;
}

/**
 * @brief 固定一个frame, 表明它不应该成为victim（即在replacer中移除该frame_id）
 * @param frame_id the id of the frame to pin
 */
void LRUReplacer::Pin(frame_id_t frame_id) {
    std::lock_guard<std::mutex> lock(latch_);
    // Todo:
    // 固定指定id的frame
    // 在数据结构中移除该frame
    

    auto it = LRUhash_.find(frame_id);//使用 LRUhash_ 中的 find 函数查找指定的 frame_id 是否在哈希表中存在。
    if (it != LRUhash_.end()) {
        // 如果在LRUlist_中找到了，说明该页面未被删除，可以固定
        LRUlist_.erase(it->second);//通过 it->second 获取在 LRUlist_ 中的迭代器位置
        LRUhash_.erase(it);
    }
}

/**
 * 取消固定一个frame, 表明它可以成为victim（即将该frame_id添加到replacer）
 * @param frame_id the id of the frame to unpin
 */
//it 是通过在 LRUhash_ 中查找 frame_id 而获得的迭代器。如果 it == LRUhash_.end()，
//这意味着在 LRUhash_ 中没有找到 frame_id，
//也就是说该帧之前没有被固定。因为如果该帧之前被固定，它的映射关系就会存在于 LRUhash_ 中。
void LRUReplacer::Unpin(frame_id_t frame_id) {
    // Todo:
    //  支持并发锁
    //  选择一个frame取消固定
    std::lock_guard<std::mutex> lock(latch_);

    auto it = LRUhash_.find(frame_id);

    if (it == LRUhash_.end()) {
        // 帧不存在于LRUlist_中，说明之前没有被固定,插入到LRUlist_中
        LRUlist_.push_front(frame_id);
        LRUhash_[frame_id] = LRUlist_.begin();
    } else {
        // 帧存在于LRUlist_中，无需执行其他操作
    }
}

/** @return replacer中能够victim的数量 */
size_t LRUReplacer::Size() {
    // Todo:
    // 改写return size
    std::lock_guard<std::mutex> lock(latch_);
    return LRUlist_.size();
    
}
