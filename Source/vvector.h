//
//  vvector.h
//  project_
//
//  Created by Ghanshyam Bhutra  and Siddhant Rath on 11/17/15.
//

#ifndef vvector_h
#define vvector_h

#include <iostream>
#include <vector>
#include <cstdint>
#include <new>
#include "polyObject.h"

#define INITIAL_BUF_SIZE 200
#define INCREASE_FACTOR 2

//vvector_allocator implements the memory handling for the vvector.
//template <typename ...args>
class vvector_allocator {
protected:
    char* _start;
    char* _pos;
    char* _end;
public:
    //Default constructor
    vvector_allocator( )  {
        //std::cout<<" "<<__FUNCTION__<<"\n";
        _start = new char[INITIAL_BUF_SIZE];
        _end = _start+INITIAL_BUF_SIZE;
        _pos = _start;
    }
    //Parameterized constructor
    vvector_allocator(size_t sz)  {
        //std::cout<<" "<<__FUNCTION__<<"\n";
        _start = new char[sz];
        _end = _start+sz;
        _pos = _start;
    }
    //copy constructor
    vvector_allocator(const vvector_allocator& copy)   {
        size_t sz = copy._end-copy._start;
        size_t szPos = copy._pos-copy._start;
        _start = new char [sz];
        _pos = _start+(szPos);
        _end = _start+sz;
        std::memcpy(_start,copy._start,szPos);
    }
    //memory allocator function
    void allocateMoreMemory() {
        try {
            size_t iPosFromStart = _pos-_start;
            size_t iMemoryReqd = _end-_start;
            char* _nstart = new char[INCREASE_FACTOR*iMemoryReqd];
            std::memcpy(_nstart,_start,iMemoryReqd);
            deallocate();
            _start = _nstart;
            _end = _start+(INCREASE_FACTOR*iMemoryReqd);
            _pos = _start+iPosFromStart;
        }
        catch (std::bad_alloc& error)  {
            std::cerr<<"Memory allocation failed!!"<< error.what();
            exit(0);
        }
    }
    
    //deallocator function
    void deallocate()   {   delete [] _start;   }
    ~vvector_allocator()    {   deallocate();   }
};


//vvector implements the actual container it inherits the memory allocation functions from vvector_allocator
template <typename ...args>
class vvector : public vvector_allocator{
private:
    std::vector<poly_object_t<has_type<equality_comparable,args...>::value,has_type<less_than_comparable,args...>::value,has_type<addable,args...>::value,has_type<ostreamable,args...>::value>> ptrs;
    
public:
    typedef typename std::vector<poly_object_t<has_type<equality_comparable,args...>::value,has_type<less_than_comparable,args...>::value,has_type<addable,args...>::value,has_type<ostreamable,args...>::value>>::iterator iterator;
    
    vvector() : vvector_allocator() {}
    vvector(size_t sz) : vvector_allocator(sz) {}
    
    vvector(vvector& copy)   :   vvector_allocator(copy)    {
        char* pos = _start;
        ptrs = copy.ptrs;
        for(int i=0;i<ptrs.size();i++)  {
            ptrs[i]._set_pointer(pos);
            pos = pos+copy.ptrs[i].byte_size();
        }
    }
    
    //push_back function expecting reference arguments
    template<typename Type>
    void push_back(Type& val)    {
        if (_pos+sizeof(Type)>_end)  {
            allocateMoreMemory();
        }
        Type* temp = new (_pos) Type(val);
        _pos = _pos+sizeof(Type);
        ptrs.push_back(temp);
        //std::cout<<__FUNCTION__<<" First:"<<(int*)_start<<" pos:"<<(int*)_pos<<" end:"<<(int*)_end<<std::endl;
    };
    //push_back function expecting rvalue arguments
    template<typename Type>
    void push_back(Type&& val)    {
        if (_pos+sizeof(Type)>_end)  {
            allocateMoreMemory();
        }
        Type* temp = new (_pos) Type(val);
        _pos = _pos+sizeof(Type);
        ptrs.push_back(temp);
        //std::cout<<__FUNCTION__<<" First:"<<(int*)_start<<" pos:"<<(int*)_pos<<" end:"<<(int*)_end<<std::endl;
    }
    //pop_back()
    void pop_back()    {
        _pos = _pos-ptrs.front().byte_size();
        ptrs.pop_back();
    };
    
    size_t size()   {   return ptrs.size();    }
    auto begin() -> decltype(ptrs.begin())    {     return ptrs.begin();    }
    auto back() -> decltype(ptrs.back())    {   return ptrs.back(); }
    auto end() -> decltype(ptrs.end())    { return ptrs.end();  }
    auto empty() -> decltype(ptrs.empty()) {    return ptrs.empty();    }
    
    //overloading the [] operator to provide random access to the elements
    poly_object_t<has_type<equality_comparable,args...>::value,has_type<less_than_comparable,args...>::value,has_type<addable,args...>::value,has_type<ostreamable,args...>::value>& operator[](size_t index)   {
        return ptrs[index];
    };
    void swap(vvector& rhs) {
        std::swap(_start,rhs._start);
        std::swap(_pos,rhs._pos);
        std::swap(_end,rhs._end);
        std::swap(ptrs,rhs.ptrs);
    }
    bool operator>(const vvector& rhs)    {
        if (ptrs.size() > rhs.ptrs.size())
            return true;
        else if (ptrs.size() < rhs.ptrs.size())
            return false;
        else    {
            if (ptrs>rhs.ptrs)
                return true;
            else
                return false;
        }
    }
    bool operator>=(const vvector& rhs)    {
        if (ptrs.size() > rhs.ptrs.size())
            return true;
        else if (ptrs.size() < rhs.ptrs.size())
            return false;
        else    {
            if (ptrs>=rhs.ptrs)
                return true;
            else
                return false;
        }
    }
    bool operator<(const vvector& rhs)    {
        if (ptrs.size() < rhs.ptrs.size())
            return true;
        else if (ptrs.size() < rhs.ptrs.size())
            return false;
        else    {
            if (ptrs<rhs.ptrs)
                return true;
            else
                return false;
        }
    }
    bool operator<=(const vvector& rhs)    {
        if (ptrs.size() < rhs.ptrs.size())
            return true;
        else if (ptrs.size() > rhs.ptrs.size())
            return false;
        else    {
            if (ptrs<=rhs.ptrs)
                return true;
            else
                return false;
        }
    }
    void clear()    {
        _pos = _start;
        ptrs.clear();
    }
    //insert functions
    template<typename T>
    void insert(int loc,T& val) {
        if (_pos+sizeof(T)>_end)
            allocateMoreMemory();
        char* location = static_cast<char*>(ptrs[loc]._get_pointer());
        std::memmove(location+sizeof(T),location,_pos-location);
        for(int i=loc;i<ptrs.size();i++)
            ptrs[i]._r_shift_pointer(sizeof(T));
        T* temp = new (location) T(val);
        ptrs.insert(ptrs.begin()+loc,temp);
    };
    template<typename T>
    void insert(int loc,T&& val) {
        char* location = static_cast<char*>(ptrs[loc]._get_pointer());
        if (_pos+sizeof(T)>_end)
            allocateMoreMemory();
        std::memmove(location+sizeof(T),location,_pos-location);
        for(int i=loc;i<ptrs.size();i++)
            ptrs[i]._r_shift_pointer(sizeof(T));
        T* temp = new (location) T(val);
        ptrs.insert(ptrs.begin()+loc,temp);
    };
   
    //erase functions
    void erase(size_t loc) {
        char* location = static_cast<char*>(ptrs[loc]._get_pointer());
        size_t sz = ptrs[loc].byte_size();
        size_t sz1 = ptrs[ptrs.size()-1].byte_size();
        std::memmove(location-sz,location,_pos-location+sz1);
        for(auto i=loc;i<ptrs.size();i++)
            ptrs[i]._l_shift_pointer(sz);
        ptrs.erase(ptrs.begin()+loc);
    }
    void erase(size_t from, size_t to) {
        char* lfrom = static_cast<char*>(ptrs[from]._get_pointer());
        char* lto = static_cast<char*>(ptrs[to]._get_pointer());
        size_t sz = lto-lfrom;
        size_t sz1 = ptrs[ptrs.size()-1].byte_size  ();
        std::memmove(lfrom,lto,_pos-lto+sz1);
        for(auto i=to;i<ptrs.size();i++)
            ptrs[i]._l_shift_pointer(sz);
        ptrs.erase(ptrs.begin()+from,ptrs.begin()+to);
    }
    
    //destructor
    ~vvector()  {
        //std::cout<<" "<<__FUNCTION__<<"\n";
    }
};


#endif /* vvector_h */
