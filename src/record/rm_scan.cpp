#include "rm_scan.h"

#include "rm_file_handle.h"
#define isStored(byteN, bitN)	\
 	((pageHandle.bitmap[byteN] >> (7 - bitN)) & 1)
// calc slot position
#define slotGet(byteN, bitN)	\
 	(byteN * 8 + bitN)
 // get position through slot
#define positionGet(slotNo)	\
 	int byteN = slotNo / 8;	\
 	int bitN = slotNo - byteN;
 // fetch page handle by pageNo
#define fetchPageHandle(pageNo)	\
 	RmPageHandle pageHandle = file_handle_->fetch_page_handle(pageNo);
 // set rid_
#define setRid(pageNo, byteN, bitN)	\
 	rid_ = {pageNo, slotGet(byteN, bitN)};

/**
 * @brief 初始化file_handle和rid
 *
 * @param file_handle
 */
RmScan::RmScan(const RmFileHandle *file_handle) : file_handle_(file_handle) {
    // Todo:
    // 初始化file_handle和rid（指向第一个存放了记录的位置）
    //获取文件中的总页数
    int numPages = file_handle_->file_hdr_.num_pages;
    //遍历文件中的每一页
    for (int pageNo = 1; pageNo < numPages; pageNo++) { // tranverse all pages
    //获取当前页的RmPageHandle。
     fetchPageHandle(pageNo);
     //获取位图的大小。
     int bitmapSize = file_handle_->file_hdr_.bitmap_size;
     //遍历当前页的位图的每一个字节。
     for (int byteN = 0; byteN < bitmapSize; byteN++)
         for (int bitN = 0; bitN < 8; bitN++) // judge by bit to find
         //如果当前位是已存储的（非空闲的），则执行以下操作：
             if (isStored(byteN, bitN)) {
                 setRid(pageNo, byteN, bitN);//设置扫描器的 rid，即表示记录的位置。
                 return;
             }
 }
}

/**
 * @brief 找到文件中下一个存放了记录的位置
 */
void RmScan::next() {
    // Todo:
    // 找到文件中下一个存放了记录的非空闲位置，用rid_来指向这个位置
    int maxpage = file_handle_->file_hdr_.num_pages;// 获取记录文件的最大页面数
    int pageno = rid_.page_no;//获取当前扫描器 rid_ 中的页号。
    int slotno = rid_.slot_no;//获取当前扫描器 rid_ 中的插槽号。
    for(;pageno < maxpage; pageno++)// 遍历页面，查找下一个包含记录的页面和插槽
    {
        //在当前页中查找下一个包含记录的插槽。
        int i = Bitmap::next_bit(1, file_handle_->fetch_page_handle(pageno).bitmap, file_handle_->file_hdr_.num_records_per_page, slotno);// 查找下一个包含记录的插槽
        if(i == file_handle_->file_hdr_.num_records_per_page)// 如果i等于页面的最大插槽数，说明没有找到下一个包含记录的插槽，将slotno设置为-1并继续下一轮查找
        {   //将插槽号 slotno 设置为-1，并继续下一轮查找。
            slotno = -1;
            continue;  
        }
        else// 找到下一个包含记录的插槽，将rid_设置为找到的页面和插槽编号
        {
            //将 rid_ 设置为找到的页面和插槽编号，然后结束函数。
            rid_.page_no = pageno;
            rid_.slot_no = i;
            return;
        }
    }
    rid_=Rid{-1,-1};// 如果没有找到下一个包含记录的位置，将rid_设置为无效值，表示扫描结束
}

/**
 * @brief ​ 判断是否到达文件末尾
 */
bool RmScan::is_end() const {
    // Todo: 修改返回值
    if(rid_.page_no == RM_NO_PAGE){
        return true;
    }else{
        return false;
    }
    
}

/**
 * @brief RmScan内部存放的rid
 */
Rid RmScan::rid() const {
    // Todo: 修改返回值
    return rid_;// 返回RmScan对象内部存放的当前的Rid，它表示扫描器的当前位置。
}