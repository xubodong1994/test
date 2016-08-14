#ifndef __IBUFFER_H
#define __IBUFFER_H

#include <sys/types.h>
#include "iobject.h"

class IEventBuffer;

class IBufferNode : public IObject {
public:
  friend class IEventBuffer;
  IBufferNode(size_t _buf_size);

  ~IBufferNode();

  size_t capacity() const;

  int resize(size_t _buf_size);

  ssize_t read(int _fd, int _how_much);

  ssize_t read(const char *_buf, int _how_much);

  ssize_t write(int _fd, int _how_much);

  ssize_t write(char *_dest_buf, int _how_much);

  size_t readable_size() const;

  size_t writable_size() const;

  std::string toString() const;

private:
  char *  _M_buf;
  char *  _M_read_ptr;
  char *  _M_write_ptr;
  char *  _M_buf_end;
};

class IEventBuffer : public IObject {
  typedef struct IBufferListNode {
    struct IBufferListNode *  _M_next_node;
    IBufferNode            *  _M_buffer_node;

    IBufferListNode(size_t _buf_size)
      : _M_buffer_node(new IBufferNode(_buf_size)),
        _M_next_node(NULL)
    { }

    ~IBufferListNode() {
      if (_M_buffer_node != NULL) {
        delete _M_buffer_node;
        _M_buffer_node = NULL;
      }
      _M_next_node = NULL;
    }
  }IBufferListNode;

  typedef struct IBufferList {
    IBufferListNode *_M_first;
    IBufferListNode *_M_last_with_data;
  }IBufferList;

private:
  static const size_t _S_default_node_size = 1024;
  static const size_t _S_max_iov = 4;
  IBufferList         _M_buffer_list;
  size_t              _M_size;

public:
  IEventBuffer();
  ~IEventBuffer();

  size_t size() const;
  ssize_t read(int _fd, int _how_much);
  ssize_t read(const char *_buf, int _how_much);
  ssize_t write(int _fd, int _how_much);
  ssize_t write(char *_buf, int _how_much);

  std::string toString() const;

private:
  int  _M_expand_buffer(IBufferListNode *_last_node, int _how_much);
  void _M_forward_read_ptr(int _how_much);
  void _M_forward_write_ptr(int _how_much);
  int  _M_setup_read_vecs(int _num_to_read, struct iovec *_iov);
  int  _M_setup_write_vecs(int _num_to_write, struct iovec *_iov);
};

#endif  // end __IBUFFER_H

