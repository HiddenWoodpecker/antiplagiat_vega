#include "Vector.h"
#include <iostream>
using namespace std;

Vector::Vector(const ValueType* rawArray, const size_t size, float coef):_size(size), _capacity(size), _multiplicativeCoef(coef){
    _data = new ValueType[_size];
    for(size_t i = 0; i < _size; i++){
        _data[i] = rawArray[i];
    }
}

Vector::Vector(const Vector& other):_size(other._size), _capacity(other._size), _multiplicativeCoef(other._multiplicativeCoef){
    _data = new ValueType[_size];
    for(size_t i = 0; i < _size; i++){
        _data[i] = other._data[i];
    }
}

Vector& Vector::operator=(const Vector& other){
    if (this == &other){
        return *this;
    }
    delete[] _data;
    _multiplicativeCoef = other._multiplicativeCoef;
    _size = other._size;
    _capacity = other._size;
    
    _data = new ValueType[_size];
    for(size_t i = 0; i < _size; i++){
        _data[i] = other._data[i];
    }
    return *this;
}

Vector::Vector(Vector&& other) noexcept:_size(other._size), _capacity(other._size), _multiplicativeCoef(other._multiplicativeCoef){
    other._multiplicativeCoef = 0.0f;
    other._size = 0;
    other._capacity = 0;
    _data = other._data;
    other._data = nullptr;
}

Vector& Vector::operator=(Vector&& other) noexcept{
    if (this == &other){
        return *this;
    }
    delete[] _data;
    _multiplicativeCoef = other._multiplicativeCoef;
    _size = other._size; 
    _capacity = other._size;
    
    other._multiplicativeCoef = 0.0f;
    other._size = 0;
    other._capacity = 0;

    _data = other._data;
    other._data = nullptr;
    return *this;
}

Vector::~Vector(){
    delete[] _data;
}

void Vector::pushBack(const ValueType& value){
    if(_capacity == 0){
        reserve(_multiplicativeCoef);
    }else if(_size == _capacity){
        reserve(_capacity*_multiplicativeCoef);
    }
    _data[_size] = value;
    _size++;
}

 void Vector::pushFront(const ValueType& value){
    if(_capacity == 0){
        reserve(_multiplicativeCoef);
    }if(_size == _capacity){
        reserve(_capacity*_multiplicativeCoef);
    }
    for(size_t i = _size; i > 0; i--){
        _data[i] = move(_data[i-1]);
    }
    _size++;
    _data[0] = value;
}

void Vector::insert(const ValueType& value, size_t pos){
    if(pos > _size){
        throw out_of_range("insert");
    }
    if(_size == _capacity){
        if(_capacity == 0){
            reserve(1);
        }else{
            reserve(_capacity*_multiplicativeCoef);
        }
    }
    for(size_t i = _size; i > pos; i--){
        _data[i] = move(_data[i-1]);
    }
    _size++;
    _data[pos] = value;
}

void Vector::insert(const ValueType* values, size_t size, size_t pos){
    if(pos > _size){
        throw out_of_range("insert");
    }
    if(_size+size > _capacity){
        reserve(_size+size);
    }
    _size += size;
    for(size_t i = _size - 1; i - size >= pos; i--){
        _data[i] = move(_data[i-size]);
    }
    for(size_t i = pos; i - pos < size; i++){
        _data[i] = values[i-pos];
    }
}

void Vector::insert(const Vector& vector, size_t pos){
    if(pos > _size){
        throw out_of_range("insert");
    }
    if(_size+vector.size() > _capacity){
        if(_capacity == 0){
            reserve(vector.size());
        }else{
            reserve(_size+vector.size());
        }
    }
    _size += vector._size;
    for(size_t i = _size - 1; i - vector.size() >= pos; i--){
        _data[i] = move(_data[i-vector.size()]);
    }
    for(size_t i = pos; i < vector.size(); i++){
        _data[i] = vector[i-pos];
    }
}

void Vector::popBack(){
    if(_size == 0)
    throw std::out_of_range("popBack");
    _size--;
}

void Vector::popFront(){
    if(_size == 0)
    throw std::out_of_range("popFront");
    _size--;
    for(size_t i = _size; i > 0; i--){
        _data[i-1] = move(_data[i]);
    }
}

void Vector::erase(size_t pos, size_t count){
    if(_size == 0){
        return;
    }else if(pos > _size){
        throw out_of_range("erase");
    }
    
    if(_size < count+pos)
        count = _size - pos;
    _size -= count;
    for (size_t i = pos; i - pos < count; i++){
        _data[i] = move(_data[count+i]);
    }
}
void Vector::eraseBetween(size_t beginPos, size_t endPos){
    if(_size == 0){
        return;
    }else if(beginPos > _size){
        throw out_of_range("erase");
    }else if(endPos > _size){
        endPos = _size;
    }
    int count = endPos - beginPos;
    erase(beginPos, count);
}

size_t Vector::size() const{
    return _size;
}
size_t Vector::capacity() const{
    return _capacity;
}
double Vector::loadFactor() const{
    return _size/_capacity;
}

ValueType& Vector::operator[](size_t idx){
    return _data[idx%_size];
}
const ValueType& Vector::operator[](size_t idx) const{
    return _data[idx%_size];
}

long long Vector::find(const ValueType& value) const{
    for(size_t i = 0; i < _size; i++){
        if (_data[i] == value)
        return i;
    }
    return -1;
}

void Vector::reserve(size_t capacity){
  if (capacity <= _capacity) {
    return;
  }
  ValueType* buffer = new ValueType[capacity];
  for (size_t i = 0; i < _size; ++i) {
    buffer[i] = _data[i];
  }
  delete[] _data;
  _data = buffer;
  _capacity = capacity;
}

void Vector::shrinkToFit(){
    _capacity = _size;
    ValueType* temp = new ValueType[_capacity];
    for (size_t i = 0; i < _size; i++) {
        temp[i] = _data[i];
    }
    delete[] _data;
    _data = temp;
}//Реализовал через доп. массив так, как realloc выдавал ошибки

Vector::Iterator::Iterator(ValueType* ptr):_ptr(ptr){}
ValueType& Vector::Iterator::operator*(){
    return *_ptr;
}
const ValueType& Vector::Iterator::operator*() const{
    return *_ptr;
}
ValueType* Vector::Iterator::operator->(){
    return _ptr;
}
const ValueType* Vector::Iterator::operator->() const{
    return _ptr;
}
Vector::Iterator Vector::Iterator::operator++(){
    ++_ptr;
    return *this;
}
Vector::Iterator Vector::Iterator::operator++(int){
    Iterator tmp = *this;
    _ptr++;
    return tmp;
}
bool Vector::Iterator::operator==(const Iterator& other) const{
    return _ptr == other._ptr;
}
bool Vector::Iterator::operator!=(const Iterator& other) const{
    return !(*this == other);
}

Vector::Iterator Vector::begin(){
    return Iterator(_data);
}
Vector::Iterator Vector::end(){
    return Iterator(_data + _size);
}
