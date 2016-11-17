#include"mempool.hpp"

#include <iostream>

namespace memalloc
{

/**
 * grow block
 **/
void mempool::grow(mempool_block* block, size_t size)
{
   std::cout << " grow " << std::endl;
   
   ///* declare new block */
   //mempool_block* new_block;
   
   /* allocate new storage */
   char* p = new char[size];
   //cout << " new pointer start = " << static_cast<void*>(&(p[0])) << endl;
   //cout << " new pointer end = " << static_cast<void*>(&(p[size-1])) << endl;
   _pool.emplace_back(p);
   
   /* init new block */
   mempool_block* new_block = reinterpret_cast<mempool_block*>( p );
   new_block = reinterpret_cast<mempool_block*>( p );
   new_block->_prev = block;
   new_block->_next = 0;
   new_block->_free = true;
   new_block->_size = size - sizeof(mempool_block);
   new_block->_root = static_cast<void*>(p);
   
   /* set next for old block */
   if(block) block->_next = new_block;
}

/**
 *
 **/
mempool_block* mempool::check_block(mempool_block* b, std::size_t size)
{
   return (b->_free && b->_size >= size) ? b : nullptr;
}

/**
 *
 **/
mempool_block* mempool::find_block_no_grow(mempool_block* b, std::size_t size)
{
   while(b)
   {
      /* find free block */
      while(!b->_free) 
      {
         /* allocate more space if needed */
         //if(!b->_next) grow(b, std::max(size + sizeof(mempool_block), _size)); 
         b = b->_next;
      }
      
      /* if free block is too small go to next block */
      if(b->_size<size) 
      {
         /* allocate more space if needed */
         //if(!b->_next) grow(b, std::max(size+sizeof(mempool_block),_size) ); 
         b = b->_next;
         continue;
      }

      /* break when we have found suitable block of memory */
      break; 
   }
   return b;
}

/**
 * Search for new block.
 **/
mempool_block* mempool::find_block(mempool_block* b, std::size_t size)
{
   while(true)
   {
      /* find free block */
      while(!b->_free) 
      {
         /* allocate more space if needed */
         if(!b->_next) grow(b, std::max(size + sizeof(mempool_block), _size)); 
         b = b->_next;
      }
      
      /* if free block is too small go to next block */
      if(b->_size<size) 
      {
         /* allocate more space if needed */
         if(!b->_next) grow(b, std::max(size+sizeof(mempool_block),_size) ); 
         b = b->_next;
         continue;
      }

      /* break when we have found suitable block of memory */
      break; 
   }
   return b;
}

/**
 * Constructor 
 **/
mempool::mempool(size_t size)
   :  _size(size)
{
   grow(nullptr, _size);
   _blocks = reinterpret_cast<mempool_block*>( *_pool.begin() );
   _last_allocated_block = _blocks;
   _last_deallocated_block = _blocks;
   //_pool.emplace_back(new char[_size]);
   //_blocks = reinterpret_cast<mempool_block*>( *_pool.begin() );
   //_blocks->_prev = 0;
   //_blocks->_next = 0;
   //_blocks->_size = _size - sizeof(mempool_block);
   //_blocks->_free = true;
   //_blocks->_root = static_cast<void*>( *_pool.begin() );
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
   /* find free block */
   //mempool_block* b = find_block(_blocks, size);
   //mempool_block* b = find_block_no_grow(_last_deallocated_block, size); 
   mempool_block* b = check_block(_last_deallocated_block, size); 
   if(!b) b = find_block(_last_allocated_block, size);
   
   mempool_block* new_block;
   /* acquire the block */
   if(b->_size - size < 2*sizeof(mempool_block))
   {
      /* if rest of block size is to small to divide, just use it all */
      b->_free = false;
      new_block = b;
   }
   else
   {
      /* else make new block and insert */
      //mempool_block* new_block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(b) + size + sizeof(mempool_block));
      new_block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(b) + size + sizeof(mempool_block));
      if(b->_next) b->_next->_prev = new_block;
      new_block->_next = b->_next;
      b->_next = new_block;
      new_block->_prev = b;
      b->_free = false;
      new_block->_size = b->_size - size - sizeof(mempool_block);
      b->_size = size;
      new_block->_free = true;
      new_block->_root = b->_root;
   }
   
   _last_allocated_block = new_block;

   return reinterpret_cast<char* >(b) + sizeof(mempool_block);
}

/**
 * release
 **/
void mempool::release(void* resource, size_t size)
{
   if(!resource) return;
   
   mempool_block* block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(resource) - sizeof(mempool_block));
   
   /* free up block */
   block->_free = true;

   /* check if both adjacent blocks are free */
   if(block->_prev && block->_next && block->_root == block->_prev->_root && block->_root == block->_next->_root)
   {
      if(block->_prev->_free && block->_next->_free)
      {
         /* update size of previous block */
         block->_prev->_size += block->_size + block->_next->_size + 2*sizeof(mempool_block);

         /* take out current and next block */
         block->_prev->_next = block->_next->_next;
         if(block->_next->_next) block->_next->_next->_prev = block->_prev;
         
         if(_last_deallocated_block != _blocks) _last_deallocated_block = block->_prev;

         /* return */
         return;
      }
   }
   
   /* check if previous block is free */
   if(block->_prev && block->_root == block->_prev->_root)
   {
      if(block->_prev->_free)
      {
         /* update size of previous block */
         block->_prev->_size += block->_size + sizeof(mempool_block);
         
         /* take current block out of list */
         block->_prev->_next = block->_next;
         if(block->_next) block->_next->_prev = block->_prev;
         
         if(_last_deallocated_block != _blocks) _last_deallocated_block = block->_prev;
         
         /* return */
         return;
      }
   }

   /* check if next block is free */
   if(block->_next && block->_root == block->_next->_root)
   {
      if(block->_next->_free)
      {
         /* update size of current block */
         block->_size += block->_next->_size + sizeof(mempool_block);

         /* take next block out of list */
         block->_next = block->_next->_next;
         if(block->_next) block->_next->_prev = block;
         
         if(_last_deallocated_block != _blocks) _last_deallocated_block = block;

         /* return */
         return;
      }
   }
}

} /* namespace memalloc */
