#pragma once

#include "ix_defs.h"
#include "ix_node_handle.h"
#include "transaction/transaction.h"

enum class Operation { FIND = 0, INSERT, DELETE };  // 三种操作：查找、插入、删除

/**
 * @brief B+树索引
 */
class IxIndexHandle {
    friend class IxScan;
    friend class IxManager;

   private:
    DiskManager *disk_manager_;
    BufferPoolManager *buffer_pool_manager_;
    int fd_;
    IxFileHdr file_hdr_;  // 存了root_page，但root_page初始化为2（第0页存FILE_HDR_PAGE，第1页存LEAF_HEADER_PAGE）
    std::mutex root_latch_;  // 用于索引并发（请自行选择并发粒度在 Tree级 或 Page级 ）

   public:
    IxIndexHandle(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager, int fd);

    // for search
    bool GetValue(const char *key, std::vector<Rid> *result, Transaction *transaction);

    IxNodeHandle *FindLeafPage(const char *key, Operation operation, Transaction *transaction);

    // for insert
    bool insert_entry(const char *key, const Rid &value, Transaction *transaction);

    IxNodeHandle *Split(IxNodeHandle *node);

    void InsertIntoParent(IxNodeHandle *old_node, const char *key, IxNodeHandle *new_node, Transaction *transaction);

    // for delete
    bool delete_entry(const char *key, Transaction *transaction);

    bool CoalesceOrRedistribute(IxNodeHandle *node, Transaction *transaction = nullptr);

    bool AdjustRoot(IxNodeHandle *old_root_node);

    void Redistribute(IxNodeHandle *neighbor_node, IxNodeHandle *node, IxNodeHandle *parent, int index);

    bool Coalesce(IxNodeHandle **neighbor_node, IxNodeHandle **node, IxNodeHandle **parent, int index,
                  Transaction *transaction);

    // 辅助函数，lab3执行层将使用
    Iid lower_bound(const char *key);

    Iid upper_bound(const char *key);

    Iid leaf_end() const;

    Iid leaf_begin() const;

   private:
    // 辅助函数
    void UpdateRootPageNo(page_id_t root) { file_hdr_.root_page = root; }

    bool IsEmpty() const { return file_hdr_.root_page == IX_NO_PAGE; }

    // for get/create node
    IxNodeHandle *FetchNode(int page_no) const;

    IxNodeHandle *CreateNode();

    // for maintain data structure
    void maintain_parent(IxNodeHandle *node);

    void erase_leaf(IxNodeHandle *leaf);

    void release_node_handle(IxNodeHandle &node);

    void maintain_child(IxNodeHandle *node, int child_idx);

    // for index test
    Rid get_rid(const Iid &iid) const;

    page_id_t getRootPageNo() {return file_hdr_.root_page; }
     /**
  * @brief 获取最右叶子节点的页面编号
  *
  * @return 最右叶子节点的page number
  * */
 page_id_t get_last_leaf() { return file_hdr_.last_leaf; }

 /**
  * @brief 设置最右节点的页面编号
  *
  * @param pageNo 要设置的编号
  * */
 void set_last_leaf(page_id_t pageNo) { file_hdr_.last_leaf = pageNo; }

 /**
  * @brief 如果若当前叶子节点是最右叶子节点，则需要更新file_hdr_.last_leaf
  * 用于节点分裂的时候
  *
  * @param {originNode} 原节点
  * @param {newNode} 分裂产生的节点
  * */
    
void updateLastLeaf(IxNodeHandle *originNode, IxNodeHandle *newNode) {
     if (originNode->GetPageNo() == get_last_leaf()) {
         set_last_leaf(newNode->GetPageNo());
     }
 }

/**
 * @brief 将原节点的键值对平均分配
 *
 * @param {newNode} 分裂出的节点
 * @param {originNode} 原节点
 * */
void evenlyDistributePairs(IxNodeHandle *newNode, IxNodeHandle *originNode) {
    int newKeyNum = originNode->GetSize() / 2;
    // 初始化新节点的page_hdr内容
    newNode->set_leaf(originNode->IsLeafPage());
    newNode->SetSize(originNode->GetSize() - newKeyNum);
    originNode->SetSize(newKeyNum);
    newNode->SetParentPageNo(originNode->GetParentPageNo());
    // 为新节点分配键值对，更新旧节点的键值对数记录
    for (int i = 0; i < newNode->GetSize(); i++) {
        newNode->set_key(i, originNode->get_key(newKeyNum + i));
        newNode->set_rid(i, *originNode->get_rid(newKeyNum + i));
    }
}

void setNewRoot(IxNodeHandle *root, IxNodeHandle *left, IxNodeHandle *right) {
    root->set_leaf(false);
    root->SetSize(2);
    root->SetParentPageNo(INVALID_PAGE_ID);
    UpdateRootPageNo(root->GetPageNo());
    root->set_key(0, left->get_key(0));
    root->set_rid(0, Rid{left->GetPageNo(), -1});  // 此处使用索引获取rid是不行的，这是为什么?
    root->set_key(1, right->get_key(0));
    root->set_rid(1, Rid{right->GetPageNo(), -1});
    // 将newRoot的孩子节点的父节点更新为newRoot
    for (int i = 0; i < root->GetSize(); i++) {
        maintain_child(root, i);
    }

    buffer_pool_manager_->UnpinPage(root->GetPageId(), true);
}

/**
 * @brief 插入节点的时候，更新相关节点的前驱和后继
 *
 * @param {originLeaf} 插入在该节点之后
 * @param {newLeaf} 要插入的节点
 * @param {keep} bool, 是否保留原节点
 * */
void insertLeaf(IxNodeHandle *originNode, IxNodeHandle *newNode, bool keep) {
    newNode->SetNextLeaf(originNode->GetNextLeaf());
    if (keep) {
        newNode->SetPrevLeaf(originNode->GetPageNo());
        originNode->SetNextLeaf(newNode->GetPageNo());
    } else  // node与其前驱合并, 什么都不用做
        ;

    if (newNode->GetNextLeaf() != INVALID_PAGE_ID) {
        // 如果下一个叶子节点存在
        auto nextNode = FetchNode(newNode->GetNextLeaf());
        nextNode->SetPrevLeaf(newNode->GetPageNo());
        // 不unpin也能通过?
        buffer_pool_manager_->UnpinPage(nextNode->GetPageId(), true);
    }
}

void splitNodeIfFull(IxNodeHandle *node, Transaction *transaction) {
    if (node->is_full()) {
        // 分裂
        IxNodeHandle *newNode = Split(node);
        // 更新父节点
        InsertIntoParent(node, newNode->get_keys(), newNode, transaction);
        // 若当前叶子节点是最右叶子节点，则需要更新file_hdr_.last_leaf
        if (node->IsLeafPage()) {
            updateLastLeaf(node, newNode);
            buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
            buffer_pool_manager_->UnpinPage(newNode->GetPageId(), true);
        }
    }
}

 /**
  * @brief 寻找node结点的兄弟结点（优先选取前驱结点）
  *
  * @param {index} node节点的位置
  * @param {parent} node节点的父亲
  * @return 返回node结点的兄弟结点（优先选取前驱结点）
  */
 IxNodeHandle *getNeighbor(int index, IxNodeHandle *parent) {
     if (index) {
         // 优先选取前驱结点
         return FetchNode(parent->get_rid(index - 1)->page_no);
     }
     return FetchNode(parent->get_rid(index + 1)->page_no);
 }

  // 交换两个节点
 void switchNode(IxNodeHandle **node, IxNodeHandle **neighbor) {
     IxNodeHandle *temp = *neighbor;
     *neighbor = *node;
     *node = temp;
 }

    
};
