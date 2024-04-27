#pragma once

#include <cstring>
#include <iostream>

namespace cow {

class string {
 private:
  struct buffer {
    explicit buffer(const char* instr) : size_(std::strlen(instr)), capacity_(size_ + 1), str_(new char[capacity_]) {
      std::strcpy(str_, instr);
    }

    explicit buffer(const buffer& buf) : size_(buf.size_), capacity_(buf.size_ + 1), str_(new char[capacity_]) {
      std::strcpy(str_, buf.str_);
    }

    buffer& operator=(const char* instr) {
      size_ = std::strlen(instr);
      capacity_ = size_ + 1;
      delete[] str_;
      str_ = new char[capacity_];
      std::strcpy(str_, instr);
    }

    buffer& operator=(const buffer& buf) {
      if (this == &buf) {
        return *this;
      }
      size_ = buf.size_;
      capacity_ = buf.size_ + 1;
      delete[] str_;
      str_ = new char[capacity_];
      std::strcpy(str_, buf.str_);
    }

    ~buffer() {
      delete[] str_;
    }

    size_t find(char c) const {
      for (size_t i = 0; i < size_; ++i) {
        if (str_[i] == c) {
          return i;
        }
      }
      return -1;
    }

    bool start_is(const char* instr) const {
      size_t i = 0;
      while (instr[i] != '\0') {
        if (instr[i] != str_[i]) {
          return false;
        }
        ++i;
      }
      return true;
    }

    size_t share_count_ = 1;
    size_t size_;
    size_t capacity_;
    char* str_;
  };

  buffer* buffer_ = nullptr;

 public:
  string() = default;
  explicit string(const char* instr) : buffer_(new buffer(instr)) {}
  string(const string& instr) : buffer_(instr.buffer_) {
    ++buffer_->share_count_;
  }

  string substr(size_t begin) const {
    return string(buffer_->str_ + begin);
  }
  
  size_t find(char c) const {
    if (buffer_ == nullptr) {
      return -1;
    }
    return buffer_->find(c);
  }
  
  bool start_is(const char* instr) {
    if (buffer_ == nullptr) {
      return *instr == '\0';
    }
    return buffer_->start_is(instr);
  }

  [[nodiscard]] size_t size() const { return buffer_ == nullptr ? 0 : buffer_->size_; }
  [[nodiscard]] char* data() const { return buffer_ == nullptr ? nullptr : buffer_->str_; }

  char operator[](size_t index) const { return buffer_->str_[index]; }
  char& operator[](size_t index) {
    if (buffer_->share_count_ != 1) {
      buffer_ = new buffer(*buffer_);
    }
    return buffer_->str_[index];
  }

  bool operator==(const char* instr) const {
    if (buffer_ == nullptr) {
      return *instr == '\0';
    }
    return std::strcmp(buffer_->str_, instr) == 0;
  }

  friend std::ostream& operator<<(std::ostream& os, const string& instr) {
    os << instr.buffer_->str_;
    return os;
  }
};

}  // namespace cow