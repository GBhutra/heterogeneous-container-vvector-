//
//  polyObject.h
//  project_
//
//  Created by Ghanshyam Bhutra  and Siddhant Rath on 11/29/15.
//

#ifndef polyObject_h
#define polyObject_h
#include <iostream>
#include <vector>
#include <cstdint>
#include <type_traits>

template <bool B, class T = void>
struct enable_if_c {    typedef T type;     };

template <class T>
struct enable_if_c<false, T> {};

template <class Cond, class T = void>
struct enable_if : public enable_if_c<Cond::value, T> {};


template <typename T, typename... Args> struct has_type {
    static const bool value = false;
};
template <typename T, typename U, typename... Args>
struct has_type<T, U, Args...> {
    static const bool value = has_type<T, Args...>::value;
};
template <typename T, typename... Args>
struct has_type<T, T, Args...> {
    static const bool value = true;
};


/*----------------- concepts ------------------*/

struct less_than_comparable	{};
struct equality_comparable	{};
struct addable	{};
struct ostreamable	{};

/*----------------- end of concepts ------------------*/

template<bool eqComp,bool lessThanComp,bool addable,bool ostreamable>
struct poly_object_t	{
public:
    poly_object_t ();
    //parameterized constructor it takes a pointer of the datatype passed as an argument.
    template<typename Type>poly_object_t (Type* t) : self(std::make_shared<model_t<Type>>(t)) {};
    
    poly_object_t (const poly_object_t& copy) : self(copy.self->clone())   { };
    
    template<typename Type>
    void operator=(Type t)  {
        auto model = dynamic_cast<model_t<Type>*>(self.get());
        model->set_value(t);
    };
    
    template<poly_object_t& p> void operator=(const poly_object_t& copy)  {
        self->swap_value(copy);
    };
    
    // getValueSize returns the size of the object pointed by data in model_t
    size_t byte_size() {return this->self->getsize(); }
    
    // getValue returns the value of the object pointed by data
    template<typename Type>
    Type& getValue() {
        model_t<Type>* d= dynamic_cast<model_t<Type>*>(self.get());
        return (d->getValue());
    }
    
    //operator overload << to print the values
    //friend std::ostream& operator<<(std::ostream& out, const poly_object_t& x)    { return x.self->print(out); };
    
    
    // _get_pointer returns the pointer to the object as a void*
    void* _get_pointer() { return self->get_pointer();};
    
    // _set_pointer updates the pointer to the object to the location in the argument
    void _set_pointer(void* location)   {self->set_pointer(location);}
    
    // _r_shift_pointer updates the pointer to the point to a memory location shifted by s units on the right
    void _r_shift_pointer(size_t s)    {self->r_shift_pointer(s);}
    
    // _l_shift_pointer updates the pointer to the point to a memory location shifted by s units on the left
    void _l_shift_pointer(size_t s)    {self->l_shift_pointer(s);}
    

    bool operator==(const poly_object_t&);
    bool operator!=(const poly_object_t& right);
    bool operator>(const poly_object_t& right);
    bool operator<(const poly_object_t& right);
    bool operator>=(const poly_object_t& right);
    bool operator<=(const poly_object_t& right);
    
    void operator+(const poly_object_t& right);
    std::ostream& operator<<(std::ostream&);

    
private:
    struct concept_t	{
    public:
        virtual std::shared_ptr<concept_t> clone() const = 0;
        virtual void* get_pointer() = 0;
        virtual void set_pointer(void* loc) = 0;
        virtual void r_shift_pointer(size_t s) = 0;
        virtual void l_shift_pointer(size_t s) = 0;
        virtual void swap_value(const poly_object_t& copy) = 0;
        
        virtual size_t getsize() const = 0;
        virtual bool equal(const poly_object_t&) = 0;
        virtual bool greaterThan(const poly_object_t&) = 0;
        virtual bool lessThan(const poly_object_t&) = 0;

        virtual void add(const poly_object_t&) = 0;
        
        virtual std::ostream& print(std::ostream&) const = 0;
        
        virtual ~concept_t() = default;
    };
    
    template<typename Type>
    struct model_t : concept_t	{
        typedef Type data_type;
    public:
        void* get_pointer()  {    return static_cast<void*>(data);    };
        void set_pointer(void* loc)   {   data = static_cast<Type*>(loc); };
        void r_shift_pointer(size_t s)   {
            void* loc = static_cast<void*>(data);
            char* loca = static_cast<char*>(loc);
            loca+=s;
            set_pointer(loca);
        };
        void l_shift_pointer(size_t s)   {
            char* loc = reinterpret_cast<char*>(data);
            loc-=s;
            set_pointer(loc);
        };
        void swap_value(const poly_object_t& copy)   {
            auto right = dynamic_cast<model_t<Type>*>(copy.self.get());
            *data = *(right->data);
        }
        size_t getsize()  const {  return(sizeof(Type));   }
        bool equal(const poly_object_t& rhs) {
            auto right = dynamic_cast<model_t<Type>*>(rhs.self.get());
            if (*data == *(right->data))
                return true;
            else
                return false;
        };
        bool greaterThan(const poly_object_t& rhs) {
            auto right = dynamic_cast<model_t<Type>*>(rhs.self.get());
            if (*data > *(right->data))
                return true;
            else
                return false;
        };
        bool lessThan(const poly_object_t& rhs) {
            auto right = dynamic_cast<model_t<Type>*>(rhs.self.get());
            if (*data < *(right->data))
                return true;
            else
                return false;
        };
        void add(const poly_object_t& rhs) {
            auto right = dynamic_cast<model_t<Type>*>(rhs.self.get());
            *data += *(right->data);
        };
        Type& getValue()    {   return *data;   }
        void set_value(poly_object_t rhs)  {
            auto right = dynamic_cast<model_t<Type>*>(rhs.self.get());
            *data = *(right->data);
        }
        void update_value(poly_object_t rhs)  {
            auto right = dynamic_cast<model_t<Type>*>(rhs.self.get());
            *data = *(right->data);
        }
        void set_value(Type t)  {
            *data = t;
        }
        std::ostream& print(std::ostream& out) const  { return out <<*(this->data); }
        model_t(Type* ptr) : data(ptr) {};
        std::shared_ptr<concept_t> clone() const    {
            return (std::shared_ptr<concept_t>(new model_t(data)));
        }
        Type* data;
    };
    std::shared_ptr<concept_t> self;
};


/********* Equality Comparable *********/
template<>bool poly_object_t<true,false,false,false>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };
template<>bool poly_object_t<true,false,false,true>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };
template<>bool poly_object_t<true,false,true,false>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };
template<>bool poly_object_t<true,false,true,true>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };
template<>bool poly_object_t<true,true,false,false>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };
template<>bool poly_object_t<true,true,false,true>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };
template<>bool poly_object_t<true,true,true,false>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };
template<>bool poly_object_t<true,true,true,true>::operator==(const poly_object_t& right)   {   return (self->equal(right));  };

template<>bool poly_object_t<true,false,false,false>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
template<>bool poly_object_t<true,false,false,true>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
template<>bool poly_object_t<true,false,true,false>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
template<>bool poly_object_t<true,false,true,true>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
template<>bool poly_object_t<true,true,false,false>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
template<>bool poly_object_t<true,true,false,true>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
template<>bool poly_object_t<true,true,true,false>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
template<>bool poly_object_t<true,true,true,true>::operator!=(const poly_object_t& right)   {   return (!self->equal(right));  };
/**************************************/

/********* Less than Comparable *********/
template<>bool poly_object_t<false,true,false,false>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };
template<>bool poly_object_t<false,true,false,true>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };
template<>bool poly_object_t<false,true,true,false>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };
template<>bool poly_object_t<false,true,true,true>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };
template<>bool poly_object_t<true,true,false,false>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };
template<>bool poly_object_t<true,true,false,true>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };
template<>bool poly_object_t<true,true,true,false>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };
template<>bool poly_object_t<true,true,true,true>::operator>(const poly_object_t& right)   {   return (self->greaterThan(right));  };

template<>bool poly_object_t<false,true,false,false>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };
template<>bool poly_object_t<false,true,false,true>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };
template<>bool poly_object_t<false,true,true,false>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };
template<>bool poly_object_t<false,true,true,true>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };
template<>bool poly_object_t<true,true,false,false>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };
template<>bool poly_object_t<true,true,false,true>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };
template<>bool poly_object_t<true,true,true,false>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };
template<>bool poly_object_t<true,true,true,true>::operator<(const poly_object_t& right)   {   return (self->lessThan(right));  };

template<>bool poly_object_t<false,true,false,false>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };
template<>bool poly_object_t<false,true,false,true>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };
template<>bool poly_object_t<false,true,true,false>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };
template<>bool poly_object_t<false,true,true,true>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };
template<>bool poly_object_t<true,true,false,false>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };
template<>bool poly_object_t<true,true,false,true>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };
template<>bool poly_object_t<true,true,true,false>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };
template<>bool poly_object_t<true,true,true,true>::operator>=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->greaterThan(right));  };

template<>bool poly_object_t<false,true,false,false>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
template<>bool poly_object_t<false,true,false,true>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
template<>bool poly_object_t<false,true,true,false>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
template<>bool poly_object_t<false,true,true,true>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
template<>bool poly_object_t<true,true,false,false>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
template<>bool poly_object_t<true,true,false,true>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
template<>bool poly_object_t<true,true,true,false>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
template<>bool poly_object_t<true,true,true,true>::operator<=(const poly_object_t& right)   {   return (self->equal(right) ? true : self->lessThan(right));  };
/**************************************/

/********* Arithmetic operations *********/
template<>void poly_object_t<false,false,true,false>::operator+(const poly_object_t& right)   {    self->add(right);  };
template<>void poly_object_t<false,false,true,true>::operator+(const poly_object_t& right)   {    self->add(right);  };
template<>void poly_object_t<false,true,true,false>::operator+(const poly_object_t& right)   {    self->add(right);  };
template<>void poly_object_t<false,true,true,true>::operator+(const poly_object_t& right)   {    self->add(right);  };
template<>void poly_object_t<true,false,true,false>::operator+(const poly_object_t& right)   {    self->add(right);  };
template<>void poly_object_t<true,false,true,true>::operator+(const poly_object_t& right)   {    self->add(right);  };
template<>void poly_object_t<true,true,true,false>::operator+(const poly_object_t& right)   {    self->add(right);  };
template<>void poly_object_t<true,true,true,true>::operator+(const poly_object_t& right)   {    self->add(right);  };
/**************************************/

/********* ostreamable *********/
template<>std::ostream& poly_object_t<false,false,false,true>::operator<<(std::ostream& out )   {    return self->print(out);  };
template<>std::ostream& poly_object_t<false,false,true,true>::operator<<(std::ostream& out )   {    return self->print(out);  };
template<>std::ostream& poly_object_t<false,true,false,true>::operator<<(std::ostream& out )   {    return self->print(out);  };
template<>std::ostream& poly_object_t<false,true,true,true>::operator<<(std::ostream& out )   {    return self->print(out);  };
template<>std::ostream& poly_object_t<true,false,false,true>::operator<<(std::ostream& out )   {    return self->print(out);  };
template<>std::ostream& poly_object_t<true,false,true,true>::operator<<(std::ostream& out )   {    return self->print(out);  };
template<>std::ostream& poly_object_t<true,true,false,true>::operator<<(std::ostream& out )   {    return self->print(out);  };
template<>std::ostream& poly_object_t<true,true,true,true>::operator<<(std::ostream& out )   {    return self->print(out);  };

std::ostream& operator<<(std::ostream&out, poly_object_t<false,false,false,true> poly)  {	return poly<<out;	}
std::ostream& operator<<(std::ostream&out, poly_object_t<false,false,true,true> poly)  {	return poly<<out;	}
std::ostream& operator<<(std::ostream&out, poly_object_t<false,true,false,true> poly)  {	return poly<<out;	}
std::ostream& operator<<(std::ostream&out, poly_object_t<false,true,true,true> poly)  {	return poly<<out;	}
std::ostream& operator<<(std::ostream&out, poly_object_t<true,false,false,true> poly)  {	return poly<<out;	}
std::ostream& operator<<(std::ostream&out, poly_object_t<true,false,true,true> poly)  {	return poly<<out;	}
std::ostream& operator<<(std::ostream&out, poly_object_t<true,true,false,true> poly)  {	return poly<<out;	}
std::ostream& operator<<(std::ostream&out, poly_object_t<true,true,true,true> poly)  {	return poly<<out;	}
/**************************************/




#endif /* polyObject_h */
