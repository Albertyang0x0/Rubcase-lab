#include "rm_file_handle.h"

/**
 * @brief 由Rid得到指向RmRecord的指针
 *
 * @param rid 指定记录所在的位置
 * @return std::unique_ptr<RmRecord>
 */
//使用 new 运算符创建一个 RmRecord 对象，同时通过页面句柄的 get_slot 方法获取记录在页面中的指针，
//并将记录大小传递给 RmRecord 构造函数。这个 RmRecord 对象将会被管理在一个 std::unique_ptr 智能指针中。
std::unique_ptr<RmRecord> RmFileHandle::get_record(const Rid &rid, Context *context) const {//用于从记录文件中获取指定位置的记录
    // Todo:
    // 1. 获取指定记录所在的page handle
    // 2. 初始化一个指向RmRecord的指针（赋值其内部的data和size）
    RmPageHandle pagehandle = fetch_page_handle(rid.page_no);// 通过指定的Rid获取相应页面的句柄
    std::unique_ptr<RmRecord> recordptr{new RmRecord(file_hdr_.record_size, pagehandle.get_slot(rid.slot_no))};// 创建一个std::unique_ptr<RmRecord>，指向一个RmRecord对象，使用页面句柄的get_slot方法获取记录的指针
    return recordptr;// 返回记录的unique_ptr指针
}

/**doubt
 * @brief 在该记录文件（RmFileHandle）中插入一条记录
 *
 * @param buf 要插入的数据的地址
 * @return Rid 插入记录的位置
 */
Rid RmFileHandle::insert_record(char *buf, Context *context) {
    // Todo:
    // 1. 获取当前未满的page handle
    // 2. 在page handle中找到空闲slot位置
    // 3. 将buf复制到空闲slot位置
    // 4. 更新page_handle.page_hdr中的数据结构
    // 注意考虑插入一条记录后页面已满的情况，需要更新file_hdr_.first_free_page_no
    RmPageHandle pagehandle = create_page_handle();// 创建一个页面句柄
    //Bitmap::first_bit 函数，该函数在位图中查找第一个为0的位，即未被占用的插槽位置
    int i = Bitmap::first_bit(0,pagehandle.bitmap,file_hdr_.num_records_per_page);// 在页面中找到可用的插槽位置
    char *slot = pagehandle.get_slot(i);// 获取该位置的插槽
    memcpy(slot, buf, file_hdr_.record_size);// 将数据从buf复制到插槽
    Bitmap::set(pagehandle.bitmap,i);// 在位图中设置相应的位，表示插槽已被占用
    pagehandle.page_hdr->num_records++;// 增加页面中的记录计数
    //如果页面中的记录数超过每页最大记录数，更新文件头的下一个可用页号
    if (pagehandle.page_hdr->num_records >= file_hdr_.num_records_per_page)// 如果页面中的记录数超过每页最大记录数，更新文件头的下一个可用页号
    {
        file_hdr_.first_free_page_no = pagehandle.page_hdr->next_free_page_no;
    }
    return Rid{pagehandle.page->GetPageId().page_no,i};// 返回表示插入记录位置的Rid
    //pagehandle.page->GetPageId().page_no 表示页面的页号，i 表示插槽的偏移量
}

/**
 * @brief 在该记录文件（RmFileHandle）中删除一条指定位置的记录
 *
 * @param rid 要删除的记录所在的指定位置
 */
void RmFileHandle::delete_record(const Rid &rid, Context *context) {
    // Todo:
    // 1. 获取指定记录所在的page handle
    // 2. 更新page_handle.page_hdr中的数据结构
    // 注意考虑删除一条记录后页面未满的情况，需要调用release_page_handle()

    // 获取指定记录所在的page handle
    RmPageHandle pageHandle = fetch_page_handle(rid.page_no);
    // 位于slot_no的record被删除
    Bitmap::reset(pageHandle.bitmap,rid.slot_no); 
    // 更新page_handle.page_hdr中的数据结构
    pageHandle.page_hdr->num_records--;

    // 检查删除一条记录后页面未满的情况
    if(pageHandle.page_hdr->num_records == 0)
    {
        release_page_handle(pageHandle);
    }
}

/**
 * @brief 更新指定位置的记录
 *
 * @param rid 指定位置的记录
 * @param buf 新记录的数据的地址
 */
void RmFileHandle::update_record(const Rid &rid, char *buf, Context *context) {
    // Todo:
    // 1. 获取指定记录所在的page handle
    // 2. 更新记录
    RmPageHandle pageHandle = fetch_page_handle(rid.page_no);//获取指定记录所在的页面句柄。
    //这里使用 Bitmap::is_set 函数检查位图中指定插槽位置是否已被占用。


    // 判断是否存在指定位置的记录
    if (Bitmap::is_set(pageHandle.bitmap, rid.slot_no)) {
    // 获取位于slot_no的record的地址
    char* recordPtr = pageHandle.get_slot(rid.slot_no);

    // 更新记录
    memcpy(recordPtr, buf, file_hdr_.record_size);
     }
}

/** -- 以下为辅助函数 -- */
/**
 * @brief 获取指定页面编号的page handle
 *
 * @param page_no 要获取的页面编号
 * @return RmPageHandle 返回给上层的page_handle
 * @note pin the page, remember to unpin it outside!
 */
RmPageHandle RmFileHandle::fetch_page_handle(int page_no) const {
    // Todo:
    // 使用缓冲池获取指定页面，并生成page_handle返回给上层
    // if page_no is invalid, throw PageNotExistError exception
    PageId page_id = { .fd = fd_, .page_no = static_cast<page_id_t>(page_no) };
    //使用缓冲池管理器 (buffer_pool_manager_) 的 FetchPage 函数获取指定页面。
    //如果页面不在缓冲池中，该函数会尝试从磁盘读取页面并将其加载到缓冲池中。
    Page* page = buffer_pool_manager_->FetchPage(page_id);

    if (!page) {
    // 如果页面不存在，可以根据你的需要抛出异常或者返回一个无效的RmPageHandle
    throw PageNotExistError("page_no Invalid", page_no);  // 假设有 PageNotExistError 异常类
    }

// 生成page_handle返回给上层
    RmPageHandle page_handle(&file_hdr_, page);

// 返回RmPageHandle
    return page_handle;
}

/**
 * @brief 创建一个新的page handle
 *
 * @return RmPageHandle
 */
RmPageHandle RmFileHandle::create_new_page_handle() {
    // Todo:
    // 1.使用缓冲池来创建一个新page
    // 2.更新page handle中的相关信息
    // 3.更新file_hdr_
    // 创建一个新的PageId，用于表示新页面
    PageId pageId = { fd_, INVALID_PAGE_ID };
    //使用缓冲池管理器 (buffer_pool_manager_) 的 NewPage 函数创建一个新的页面，
    //并将新页面的 PageId 赋给 pageId
    Page* page = buffer_pool_manager_->NewPage(&pageId);

// 增加文件头中的页面数量
    ++file_hdr_.num_pages;
// ：使用获取到的新页面和更新后的文件头信息构建 RmPageHandle。
    RmPageHandle page_handle(&file_hdr_, page);

    return page_handle;
}

/**
 * @brief 创建或获取一个空闲的page handle
 *
 * @return RmPageHandle 返回生成的空闲page handle
 * @note pin the page, remember to unpin it outside!
 */
RmPageHandle RmFileHandle::create_page_handle() {
    // Todo:
    // 1. 判断file_hdr_中是否还有空闲页
    //     1.1 没有空闲页：使用缓冲池来创建一个新page；可直接调用create_new_page_handle()
    //     1.2 有空闲页：直接获取第一个空闲页
    // 2. 生成page handle并返回给上层

    //获取文件头中的第一个空闲页的页号。
    int freeNo = file_hdr_.first_free_page_no;

    if (freeNo < 0) { // no free page
    return create_new_page_handle();//创建一个新的页面句柄
    }
//如果存在空闲页，则调用 fetch_page_handle 函数获取指定页号的页面句柄
    RmPageHandle pageHandle = fetch_page_handle(freeNo);
// 将获取到的页面句柄返回给上层
    return pageHandle;
}

/**
 * @brief 当page handle中的page从已满变成未满的时候调用
 *
 * @param page_handle
 * @note only used in delete_record()
 */
void RmFileHandle::release_page_handle(RmPageHandle &page_handle) {
    // Todo:
    // 当page从已满变成未满，考虑如何更新：
    // 1. page_handle.page_hdr->next_free_page_no
    // 2. file_hdr_.first_free_page_no

    //获取当前页面的页号。
    int pageNo = page_handle.page->GetPageId().page_no;
    //将当前页面的 next_free_page_no 设置为文件头中的第一个空闲页号。
    //这是因为当前页面即将成为空闲页，所以它的下一个空闲页就是文件头中的第一个空闲页。
    page_handle.page_hdr->next_free_page_no = file_hdr_.first_free_page_no;
    //将文件头中的第一个空闲页号更新为当前释放的页面的页号。现在，当前页面成为了文件头中的第一个空闲页。
    file_hdr_.first_free_page_no = pageNo;
}

/**
 * @brief 用于事务的rollback操作
 *
 * @param rid record的插入位置
 * @param buf record的内容
 */
void RmFileHandle::insert_record(const Rid &rid, char *buf) {
    if (rid.page_no < file_hdr_.num_pages) {
        create_new_page_handle();
    }
    RmPageHandle pageHandle = fetch_page_handle(rid.page_no);
    Bitmap::set(pageHandle.bitmap, rid.slot_no);
    pageHandle.page_hdr->num_records++;
    if (pageHandle.page_hdr->num_records == file_hdr_.num_records_per_page) {
        file_hdr_.first_free_page_no = pageHandle.page_hdr->next_free_page_no;
    }

    char *slot = pageHandle.get_slot(rid.slot_no);
    memcpy(slot, buf, file_hdr_.record_size);

    buffer_pool_manager_->UnpinPage(pageHandle.page->GetPageId(), true);
}
