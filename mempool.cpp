#include"mempool.h"

namespace memalloc
{

/**
 * grow block
 **/
void mempool::grow(mempool_block* block, size_t size)
{
   //cout << " grow " << endl;
   
   /* declare new block */
   mempool_block* new_block;
   
   /* allocate new storage */
   char* p = new char[size];
   //cout << " new pointer start = " << static_cast<void*>(&(p[0])) << endl;
   //cout << " new pointer end = " << static_cast<void*>(&(p[size-1])) << endl;
   _pool.push_back(p);
   
   /* init new block */
   //mempool_block* new_block = reinterpret_cast<mempool_block*>( p );
   new_block = reinterpret_cast<mempool_block*>( p );
   new_block->_prev=block;
   new_block->_next=0;
   new_block->_free = true;
   new_block->_size = size-sizeof(mempool_block);
   new_block->_root = static_cast<void*>(p);
   
   /* set next for old block */
   block->_next = new_block;
}

/**
 * Constructor 
 **/
mempool::mempool(size_t size): _size(size)
{
   _pool.push_back(new char[_size]);
   _blocks = reinterpret_cast<mempool_block*>( *_pool.begin() );
   _blocks->_prev = 0;
   _blocks->_next = 0;
   _blocks->_size = _size-sizeof(mempool_block);
   _blocks->_free = true;
   _blocks->_root = static_cast<void*>( *_pool.begin() );
}

/**
 * Destructor
 **/
mempool::~mempool()
{
   for_each(_pool.begin(), _pool.end(), killer());
}

/**
 * acquire
 **/
void* mempool::acquire(size_t size)
{
   //cout << " doign acquire " << endl;
   mempool_block* b=_blocks;
   while(1)
   {
      //cout << " b: " << b << endl;
      while(!b->_free) /* find free block */
      {
         //cout << " in while " << endl;
         if(!b->_next) grow(b, std::max(size+sizeof(mempool_block), _size) ); /* allocate more space if needed */
         b=b->_next;
      }
      if(b->_size<size) /* if free block is too small go to next block */
      {
         //cout << " in if " << endl;
         //cout << b->_next << endl;
         //cout << b->_next->_free << endl;
         if(!b->_next) grow(b, std::max(size+sizeof(mempool_block),_size) ); /* allocate more space if needed */
         b = b->_next;
         continue;
      }
      break; /* break when we have found suitable block of memory */
   }
   
   if(b->_size - size < 2*sizeof(mempool_block))
   {
      /* if rest of block size is to small to divide, just use it all */
      //cout << " block is small " << endl;
      b->_free = false;
      return reinterpret_cast<char* >(b) + sizeof(mempool_block);
   }
   else
   {
      /* else make new block and insert */
      mempool_block* new_block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(b) \
         + size + sizeof(mempool_block));
      if(b->_next) b->_next->_prev = new_block;
      new_block->_next=b->_next;
      b->_next = new_block;
      //cout << " size = " << size << endl;
      //cout << " b = " << b << endl;
      //cout << " b->_size = " << b->_size << endl;
      //cout << " &new_block->_next = " << &(new_block->_next) << endl;
      //cout << " &new_block->_prev = " << &(new_block->_prev) << endl;
      //cout << " new_block->_prev = " << new_block->_prev << endl;
      new_block->_prev=b;
      //cout << " new_block->_prev = " << new_block->_prev << endl;
      b->_free = false;
      new_block->_size=b->_size - size - sizeof(mempool_block);
      b->_size = size;
      new_block->_free=true;
      new_block->_root=b->_root;

      return reinterpret_cast<char* >(b)+sizeof(mempool_block);
   }
}

/**
 * release
 **/
void mempool::release(void* resource, size_t size)
{
   if(!resource) return;
   //cout << " doign release ! " << endl;
   
   mempool_block* block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(resource) \
         - sizeof(mempool_block));
   
   /* free up block */
   //block->_free = true;

   /* check if adjacent blocks are free */
   if(block->_prev && block->_next && block->_root == block->_prev->_root && block->_root == block->_next->_root)
      if(block->_prev->_free && block->_next->_free)
      {
         /* update size of previous block */
         block->_prev->_size += block->_size + block->_next->_size + 2*sizeof(mempool_block);

         /* take out current and next block */
         block->_prev->_next = block->_next->_next;
         if(block->_next->_next) block->_next->_next->_prev = block->_prev;
         
         /* return */
         return;
      }
   if(block->_prev && block->_root == block->_prev->_root)
      if(block->_prev->_free)
      {
         /* update size of previous block */
         block->_prev->_size += block->_size + sizeof(mempool_block);
         
         /* take current block out of list */
         block->_prev->_next = block->_next;
         if(block->_next) block->_next->_prev = block->_prev;
         
         block->_free=true;

         /* return */
         return;
      }
   if(block->_next && block->_root == block->_next->_root)
      if(block->_next->_free)
      {
         /* update size of current block */
         block->_size += block->_next->_size + sizeof(mempool_block);

         /* take next block out of list */
         block->_next = block->_next->_next;
         if(block->_next) block->_next->_prev = block;
         
         block->_free=true;

         /* return */
         return;
      }
   block->_free = true;
}

} /* namespace memalloc */
