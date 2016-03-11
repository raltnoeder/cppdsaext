#ifndef DSAEXT_H
#define	DSAEXT_H

#include <cstddef>
#include <stdexcept>

namespace dsaext
{
    template<typename K, typename V>
    class Map
    {
      public:
        typedef struct entry_s
        {
            K* key;
            V* value;
        }
        entry;

        class Node
        {
          public:
            virtual K* get_key() const = 0;
            virtual V* get_value() const = 0;

            virtual ~Node()
            {
            }
        };

        virtual ~Map()
        {
        }

        virtual V* get(const K* key) const = 0;
        virtual void insert(K* key, V* value) = 0;
        virtual void remove(const K* key) = 0;
        virtual void clear() = 0;
        virtual size_t get_size() const = 0;
    };

    template<typename T>
    class QIterator
    {
        public:
           virtual ~QIterator()
           {
           }
           virtual T*   next() = 0;
           virtual bool has_next() const = 0;
           virtual size_t get_size() const = 0;
    };

    class DuplicateInsertException : public std::exception
    {
        public:
            DuplicateInsertException();
            virtual ~DuplicateInsertException() noexcept;
            
            DuplicateInsertException(const DuplicateInsertException& orig) = default;
            DuplicateInsertException& operator=(const DuplicateInsertException& orig) = default;
            DuplicateInsertException(DuplicateInsertException&& orig) = default;
            DuplicateInsertException& operator=(DuplicateInsertException&& orig) = default;
        private:
    };
}

#endif	/* DSAEXT_H */
