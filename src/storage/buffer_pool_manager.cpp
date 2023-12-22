#include "buffer_pool_manager.h"

using std::cout;
using std::endl;

#define getParams(page) 						\
	int fd = page->GetPageId().fd;			\
    int page_no = page->GetPageId().page_no;      \
    char* offset = page->GetData();			\
    int num_bytes = PAGE_SIZE;
/**
 * @brief 从free_list或replacer中得到可淘汰帧页的 *frame_id
 * @param frame_id 帧页id指针,返回成功找到的可替换帧id
 * @return true: 可替换帧查找成功 , false: 可替换帧查找失败
 */
// Todo:
    // 1 使用BufferPoolManager::free_list_判断缓冲池是否已满需要淘汰页面
    // 1.1 未满获得frame
    // 1.2 已满使用lru_replacer中的方法选择淘汰页面
bool BufferPoolManager::FindVictimPage(frame_id_t *frame_id) {
    
    if (free_list_.empty()) { // free page exists, get frame
    	int frameId;
    	if (replacer_->Victim(&frameId)) {//如果空闲列表为空，函数尝试使用替换器找到一个牺牲页
    		
    		page_table_.erase(pages_[frameId].GetPageId());//删除所需的空闲页（如果找到了牺牲页）
    		*frame_id = frameId;
    		return true;//返回帧ID:
    	}
    } else { //如果空闲列表不为空，表示有可用的空闲页。
    	*frame_id = free_list_.front();
        free_list_.pop_front();//如果有可用的空闲页，函数从空闲列表的前面选择一个页面，将frame_id设置为其ID，并将其从空闲列表中移除
        return true;
    }
    return false;
}

/**
 * @brief 更新页面数据, 为脏页则需写入磁盘，更新page元数据(data, is_dirty, page_id)和page table
 *
 * @param page 写回页指针
 * @param new_page_id 写回页新page_id
 * @param new_frame_id 写回页新帧frame_id
 */
void BufferPoolManager::UpdatePage(Page *page, PageId new_page_id, frame_id_t new_frame_id) {
    // Todo:
    // 1 如果是脏页，写回磁盘，并且把dirty置为false
    // 2 更新page table
    // 3 重置page的data，更新page id
    if(page->IsDirty())// 如果页面是脏页，需要将其数据写回磁盘
    {
        page->is_dirty_ = false;// 标记页面为非脏页
        //将脏页的数据写回磁盘，通过disk_manager_->write_page将页面数据写入与页面相关联的文件（使用页面ID的文件描述符和页面号）。
        disk_manager_->write_page(page->GetPageId().fd, page->GetPageId().page_no, page->GetData(), PAGE_SIZE);
    }
    page_table_.erase(page->GetPageId());// 从页表中删除旧的页面ID映射
    if(new_page_id.page_no != INVALID_PAGE_ID)// 如果新页面ID不是无效的，将新的页面ID和帧ID插入页表中
    {
        page_table_.insert(std::make_pair(new_page_id, new_frame_id));
    }
    // 重置页面的内存和ID
    page->ResetMemory();
    page->id_ = new_page_id;
}


/**
 * Fetch the requested page from the buffer pool.
 * 如果页表中存在page_id（说明该page在缓冲池中），并且pin_count++。
 * 如果页表不存在page_id（说明该page在磁盘中），则找缓冲池victim page，将其替换为磁盘中读取的page，pin_count置1。
 * @param page_id id of page to be fetched
 * @return the requested page
 */

// Todo:从缓冲池中获取指定页面
    // 0.     lock latch
    // 1.     Search the page table for the requested page (P).
    // 1.1    If P exists, pin it and return it immediately.
    // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
    //        Note that pages are always found from the free list first.
    // 2.     If R is dirty, write it back to the disk.
    // 3.     Delete R from the page table and insert P.
    // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
Page *BufferPoolManager::FetchPage(PageId page_id) {
    std::scoped_lock lock{latch_};
    
    auto it = page_table_.find(page_id);//在页表中查找指定 page_id 是否已经在缓冲池中。如果在，表示该页面已经在缓冲池中，可以直接返回
    if (it != page_table_.end()) { // page table history exists
    	// 将缓冲池中对应的帧进行固定（pin）操作。固定表示该帧不可被替换，保持在缓冲池中
    	replacer_->Pin(it->second);
        //增加对应页面的引用计数。引用计数用于跟踪有多少指针指向该页面
    	++pages_[it->second].pin_count_;
    	
    	return &pages_[it->second];
    }
    
    int target;
    //通过 FindVictimPage 函数找到一个可用的帧。如果找到，说明有可用的帧，可以将新的页面加载到这个帧中。
    if (FindVictimPage(&target)){ 
    
    	Page* page = &pages_[target];
    	if (page->IsDirty())
    		// 需要将其写回磁盘
    		UpdatePage(page, page_id, target);
    	else page->id_ = page_id;
    	
    	// 获取页面参数，然后通过 disk_manager_ 从磁盘读取页面的内容到帧中
    	getParams(page);
    	disk_manager_->read_page(fd, page_no, offset, num_bytes);
    	
    	// 将新读取的页面加入页表，并将对应的帧固定，并增加引用计数
    	page_table_.insert({page_id, target});
    	replacer_->Pin(target);
    	++pages_[target].pin_count_;
    	return &pages_[target];//返回指向缓冲池中帧的指针
    }
    
    // no available frame
    return nullptr;
}

/**
 * Unpin the target page from the buffer pool. 取消固定pin_count>0的在缓冲池中的page
 * @param page_id id of page to be unpinned
 * @param is_dirty true if the page should be marked as dirty, false otherwise
 * @return false if the page pin count is <= 0 before this call, true otherwise
 */
// Todo:
    // 0. lock latch
    // 1. try to search page_id page P in page_table_
    // 1.1 P在页表中不存在 return false
    // 1.2 P在页表中存在 如何解除一次固定(pin_count)
    // 2. 页面是否需要置脏
bool BufferPoolManager::UnpinPage(PageId page_id, bool is_dirty) {
//取消页面的固定（unpin）操作，并根据需要标记页面为脏页（dirty）
    std::scoped_lock lock{latch_};
    auto it = page_table_.find(page_id);

    //如果在页表中找不到给定的页面 ID，说明该页面不在缓冲池中
    if (it == page_table_.end()) {
        return false;
    }

    //获取页面 ID 对应的帧 ID
    int frame_id = it->second;
    Page* page = &pages_[frame_id];

    //如果页面的引用计数大于 0，表示该页面被固定，可以进行取消固定操作
    if (page->pin_count_ > 0) {
        --page->pin_count_;//减少页面的引用计数，表示取消固定。
    } else {
        
        return false;
    }

    //如果需要标记页面为脏页
    if (is_dirty) {
        page->is_dirty_ = true;
    }

    //如果取消固定后，页面的引用计数为 0，表示该页面不再被使用，可以进行替换
    if (page->pin_count_ == 0) {
        replacer_->Unpin(frame_id);
    }

    return true;

}

/**
 * Flushes the target page to disk. 将page写入磁盘；不考虑pin_count
 * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
 * @return false if the page could not be found in the page table, true otherwise
 */
// Todo:
    // 0. lock latch
    // 1. 页表查找
    // 2. 存在时如何写回磁盘
    // 3. 写回后页面的脏位
    // Make sure you call DiskManager::WritePage!

//刷新（flush）缓冲池中指定页面到磁盘的操作
bool BufferPoolManager::FlushPage(PageId page_id) {
    
    std::scoped_lock lock{latch_};

    // 1. 页表查找
    auto it = page_table_.find(page_id);

    // 1.1 不存在于页表中，返回false
    if (it == page_table_.end()) {
        return false;
    }

    // 2. 存在时如何写回磁盘
    int frame_id = it->second;
    Page* page = &pages_[frame_id];

    // 调用 DiskManager::WritePage 将页面写回磁盘
    disk_manager_->write_page(page->GetPageId().fd, page->GetPageId().page_no, page->GetData(), PAGE_SIZE);

    // 3. 写回后页面的脏位
    page->is_dirty_ = false;

    return true;
}

/**
 * Creates a new page in the buffer pool. 相当于从磁盘中移动一个新建的空page到缓冲池某个位置
 * @param[out] page_id id of created page
 * @return nullptr if no new pages could be created, otherwise pointer to new page
 */
Page *BufferPoolManager::NewPage(PageId *page_id) {
    // Todo:
    // 0.   lock latch
    // 1.   Make sure you call DiskManager::AllocatePage!
    // 2.   If all the pages in the buffer pool are pinned, return nullptr.
    // 3.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
    // 4.   Update P's metadata, zero out memory and add P to the page table. pin_count set to 1.
    // 5.   Set the page ID output parameter. Return a pointer to P.
    std::scoped_lock lock{latch_};
    frame_id_t frame_id = -1;
    if (!FindVictimPage(&frame_id))// 如果找不到victim页面，返回nullptr
    {
        return nullptr;
    }
    page_id->page_no = disk_manager_->AllocatePage(page_id->fd);// 为新页面分配一个page_no，并将其存储在page_id中
    Page *page = &pages_[frame_id];// 获取新页面的指针
    UpdatePage(page, *page_id, frame_id);// 更新新页面的数据和元数据
    replacer_->Pin(frame_id);// 固定（pin）新页面
    page->pin_count_ = 1;// 固定（pin）新页面
    return page;// 返回新页面的指针
}

/**
 * @brief Deletes a page from the buffer pool.
 * @param page_id id of page to be deleted
 * @return false if the page exists but could not be deleted, true if the page didn't exist or deletion succeeded
 */
 // Todo:
    // 0.   lock latch
    // 1.   Make sure you call DiskManager::DeallocatePage!
    // 2.   Search the page table for the requested page (P).
    // 2.1  If P does not exist, return true.
    // 2.2  If P exists, but has a non-zero pin-count, return false. Someone is using the page.
    // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free
    // list.
bool BufferPoolManager::DeletePage(PageId page_id) {
   
    std::scoped_lock lock{latch_};

    //调用 DiskManager 的 DeallocatePage 方法，释放磁盘上对应页面的存储空间。
    disk_manager_->DeallocatePage(page_id.page_no);

    //在页表中查找给定页面 ID 是否存在。
    auto it = page_table_.find(page_id);

    //如果在页表中找不到给定的页面 ID，说明该页面在缓冲池中不存在，返回 true
    if (it == page_table_.end()) {
        return true;
    }

   //获取页面 ID 对应的帧 ID
    int frame_id = it->second;//获取帧的指针
    Page* page = &pages_[frame_id];

    if (page->pin_count_ > 0) {////如果页面的引用计数大于 0（即页面被固定），返回 false
        return false;
    }

   
    page_table_.erase(it);
    page->ResetMemory();
    free_list_.push_back(frame_id);//将帧 ID 放入空闲列表。

    return true;

}

/**
 * @brief Flushes all the pages in the buffer pool to disk.
 *
 * @param fd 指定的diskfile open句柄
 */
void BufferPoolManager::FlushAllPages(int fd) {
    // example for disk write
    std::scoped_lock lock{latch_};
    for (size_t i = 0; i < pool_size_; i++) {
        Page *page = &pages_[i];
        if (page->GetPageId().fd == fd && page->GetPageId().page_no != INVALID_PAGE_ID) {
            disk_manager_->write_page(page->GetPageId().fd, page->GetPageId().page_no, page->GetData(), PAGE_SIZE);
            page->is_dirty_ = false;
        }
    }
}