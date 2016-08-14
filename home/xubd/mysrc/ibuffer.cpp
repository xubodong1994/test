#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "ibuffer.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include "iutil.h"

IBufferNode::IBufferNode(size_t _buf_size)
{
  if (_buf_size == 0) {
    _M_buf = _M_read_ptr = _M_write_ptr = _M_buf_end = 0;
  }
  else {
    _M_buf = new char[_buf_size];
    if (_M_buf != NULL) {
      _M_read_ptr = _M_write_ptr = _M_buf;
      _M_buf_end  = _M_buf + _buf_size;
    }
  }
}

size_t IBufferNode::capacity() const
{
  return (size_t)(_M_buf_end - _M_buf);
}

size_t IBufferNode::readable_size() const
{
  return (size_t)(_M_buf_end - _M_read_ptr);
}

size_t IBufferNode::writable_size() const
{
  return (size_t)(_M_read_ptr - _M_write_ptr);
}

std::string IBufferNode::toString() const
{
  std::string _obj_str;
  char        _buf[4096];

  snprintf(_buf, writable_size(), "{size:%ld content:%s}", capacity(), _M_write_ptr);
  _obj_str = _buf;

  return _obj_str;
}

int IBufferNode::resize(size_t _buf_size)
{
  int _ret_code = 0;
  char *_new_buf = NULL;

  if (_buf_size > this->capacity()) {
    _new_buf = new char[_buf_size];
    if (_new_buf != NULL) {
      memcpy(_new_buf, _M_write_ptr, writable_size());
      delete _M_buf;
      _M_read_ptr = _new_buf + writable_size();
      _M_buf = _new_buf;
      _M_write_ptr = _new_buf;
      _M_buf_end = _new_buf + _buf_size;
    }
    else {
      _ret_code = -1;
    }
  }
  return _ret_code;
}

ssize_t IBufferNode::read(int _fd, int _how_much)
{
  int     _num_to_read = 0;
  ssize_t _num_read = 0;

  if (_how_much == -1)
    _how_much = readable_size();

  _num_to_read = std::min((size_t)_how_much, readable_size());
  if (_num_to_read > 0)
    _num_read = ::read(_fd, _M_read_ptr, _num_to_read);

  if (_num_read > 0)
    _M_read_ptr += _num_read;

  return _num_read;
}

ssize_t IBufferNode::write(int _fd, int _how_much)
{
  int     _num_to_write = 0;
  ssize_t _num_wrote = 0;

  if (_how_much == -1)
    _how_much = writable_size();

  _num_to_write = std::min((size_t)_how_much, writable_size());
  if (_num_to_write > 0)
    _num_wrote = ::write(_fd, _M_write_ptr, _num_to_write);

  if (_num_wrote > 0)
    _M_write_ptr += _num_wrote;

  return _num_wrote;
}

ssize_t IBufferNode::read(const char *_buf, int _how_much)
{
  ssize_t _num_read = 0;

  if (_how_much == -1)
    _how_much = readable_size();

  _num_read = std::min((size_t)_how_much, readable_size());
  memcpy(_M_read_ptr, _buf, _num_read);
  _M_read_ptr += _num_read;

  return _num_read;
}

ssize_t IBufferNode::write(char *_buf, int _how_much)
{
  ssize_t _num_wrote = 0;

  if (_how_much == -1)
    _how_much = writable_size();

  _num_wrote = std::min((size_t)_how_much, writable_size());
  memcpy(_buf, _M_write_ptr, _num_wrote);
  _M_write_ptr += _num_wrote;

  return _num_wrote;
}

IBufferNode::~IBufferNode()
{
  if (_M_buf != NULL)
    delete _M_buf;

  _M_buf = NULL;
  _M_read_ptr = NULL;
  _M_write_ptr = NULL;
  _M_buf_end = NULL;
}

IEventBuffer::IEventBuffer()
{
  _M_buffer_list._M_first = NULL;
  _M_buffer_list._M_last_with_data = NULL;
  _M_size = 0;
}

size_t IEventBuffer::size() const
{
  return _M_size;
}

int IEventBuffer::_M_expand_buffer(IBufferListNode *_last_node, int _how_much)
{
  IBufferListNode *_new_node = NULL;
  int _real_alloc_size = _S_default_node_size;

  while (_how_much > _real_alloc_size)
    _real_alloc_size <<= 1;
  _new_node = new IBufferListNode(_real_alloc_size);

  if (_new_node == NULL)
    return -1;

  if (_last_node == NULL) {
    _M_buffer_list._M_first = _new_node;
    _M_buffer_list._M_last_with_data = _new_node;
  }
  else {
    _last_node->_M_next_node = _new_node;
  }

  return 0;
}

int IEventBuffer::_M_setup_read_vecs(int _num_to_read, struct iovec *_read_iov)
{
  int               _iov_len = 0;
  int               _num_available = 0;
  int               _num_readable = 0;
  char *            _read_ptr;
  IBufferListNode * _next_node = NULL;
  IBufferListNode * _last_node = NULL;

  //__assert((_iov != NULL));
  _next_node = _M_buffer_list._M_last_with_data;
  if (_next_node && _next_node->_M_buffer_node->readable_size() == 0) {
    _last_node = _next_node;
    _next_node = _next_node->_M_next_node;
  }

  while(_iov_len < _S_max_iov && _next_node && (_num_available < _num_to_read)) {
    _read_ptr = _next_node->_M_buffer_node->_M_read_ptr;
    _num_readable = _next_node->_M_buffer_node->readable_size();

    _read_iov[_iov_len].iov_base = _read_ptr;
    _read_iov[_iov_len].iov_len = _num_readable;
    ++_iov_len;

    _last_node = _next_node;
    _next_node = _next_node->_M_next_node;
    _num_available += _num_readable;
  }

  if (_num_available < _num_to_read) {
    if (_iov_len == _S_max_iov) {
      //_num_to_read = _num_available;
    }
    else if (_M_expand_buffer(_last_node, _num_to_read - _num_available) == 0) {
      if (_last_node)
        _last_node = _last_node->_M_next_node;
      else
        _last_node = _M_buffer_list._M_last_with_data;
      _read_iov[_iov_len].iov_base = _last_node->_M_buffer_node->_M_read_ptr;
      _read_iov[_iov_len].iov_len  = _num_to_read - _num_available;

      ++_iov_len;

      //_num_available = _num_to_read;
    }
  }

  return _iov_len;
}

int IEventBuffer::_M_setup_write_vecs(int _num_to_write, struct iovec *_write_iov)
{
  int               _iov_len = 0;
  int               _num_writable = 0;
  int               _num_available = 0;
  char *            _write_ptr = NULL;
  IBufferListNode * _next_node = NULL;

  _next_node = _M_buffer_list._M_first;
  //__assert((_next_node != NULL));
  //__assert((_num_to_write <= _M_size));

  while (_iov_len < _S_max_iov && _next_node && _num_available < _num_to_write) {
    _write_ptr = _next_node->_M_buffer_node->_M_write_ptr;
    _num_writable = _next_node->_M_buffer_node->writable_size();

    _write_iov[_iov_len].iov_base = _write_ptr;
    _write_iov[_iov_len].iov_len = _num_writable;
    ++_iov_len;

    _num_available += _num_writable;
    _next_node = _next_node->_M_next_node;
  }

  if (_num_available > _num_to_write) {
    _write_iov[_iov_len - 1].iov_len -= (_num_available - _num_to_write);
  }

  return _iov_len;
}

ssize_t IEventBuffer::read(int _fd, int _how_much)
{
  ssize_t       _num_read = 0;
  int           _num_readable = 0;
  int           _num_to_read = 0;
  int           _iov_len = 0;
  struct iovec  _read_iov[_S_max_iov] = {{0, 0}};

  if ((_num_readable = n_bytes_readable(_fd)) < 0)
    return -1;

  if (_num_readable == 0)
    _num_to_read = _how_much;
  else
    _num_to_read = std::min(_num_readable, _how_much);

  IBufferListNode *_last_with_data = _M_buffer_list._M_last_with_data;
  if (_last_with_data && _last_with_data->_M_buffer_node->readable_size() >= _num_to_read) {
    _num_read = _last_with_data->_M_buffer_node->read(_fd, _num_to_read);
  }
  else {
    _iov_len = _M_setup_read_vecs(_num_to_read, _read_iov);
    if (_iov_len > 0) {
      _num_read = readv(_fd, _read_iov, _iov_len);
    }
  }

  if (_num_read > 0) {
    if (_iov_len > 0)
      _M_forward_read_ptr(_num_read);
    _M_size += _num_read;
  }

  return _num_read;
}

void IEventBuffer::_M_forward_read_ptr(int _how_much)
{
  int _num_to_forward = _how_much;
  int _num_readable = 0;
  IBufferListNode *_last_with_data = _M_buffer_list._M_last_with_data;

  //__assert((_last_with_data != NULL));

  while (true) {
    _num_readable = _last_with_data->_M_buffer_node->readable_size();
    if (_num_to_forward <= _num_readable) {
      _last_with_data->_M_buffer_node->_M_read_ptr += _num_to_forward;
      _M_buffer_list._M_last_with_data = _last_with_data;
      break;
    }
    _last_with_data->_M_buffer_node->_M_read_ptr += _num_readable;
    _last_with_data = _last_with_data->_M_next_node;
    _num_to_forward -= _num_readable;
  }
}

void IEventBuffer::_M_forward_write_ptr(int _how_much)
{
  int _num_to_forward = _how_much;
  int _num_writable = 0;
  IBufferListNode *_first_node = _M_buffer_list._M_first;

  while (true) {
    _num_writable = _first_node->_M_buffer_node->writable_size();
    if (_num_to_forward <= _num_writable) {
      _first_node->_M_buffer_node->_M_write_ptr += _num_to_forward;
      //_M_buffer_list._M_first = _first_node;
      break;
    }
    _M_buffer_list._M_first = _first_node->_M_next_node;
    delete _first_node;
    _first_node = _M_buffer_list._M_first;
    _num_to_forward -= _num_writable;
  }
}

ssize_t IEventBuffer::read(const char *_buf, int _how_much)
{
  ssize_t       _num_read = 0;
  int           _num_readable = 0;
  int           _num_to_read = 0;
  int           _iov_len = 0;
  struct iovec  _read_iov[_S_max_iov] = {{0, 0}};
  struct iovec *_iov_ptr;

  if (_how_much < 0)
    return -1;

  _num_to_read = _how_much; //std::min(_num_readable, _how_much);

  IBufferListNode *_last_with_data = _M_buffer_list._M_last_with_data;
  if (_last_with_data && _last_with_data->_M_buffer_node->readable_size() >= _num_to_read) {
    _num_read = _last_with_data->_M_buffer_node->read(_buf, _num_to_read);
  }
  else {
    _iov_len = _M_setup_read_vecs(_num_to_read, _read_iov);
    for (int _i = 0; _i < _iov_len; ++_i) {
      _iov_ptr = _read_iov + _i;
      memcpy(_iov_ptr->iov_base, _buf + _num_read, _iov_ptr->iov_len);
      _num_read += _iov_ptr->iov_len;
    }
  }

  if (_num_read > 0) {
    if (_iov_len > 0)
      _M_forward_read_ptr(_num_read);
    _M_size += _num_read;
  }

  return _num_read;
}

ssize_t IEventBuffer::write(int _fd, int _how_much)
{
  ssize_t       _num_wrote = 0;
  ssize_t       _num_to_write = 0;
  int           _iov_len = 0;
  IBufferNode * _last_buffer_node = NULL;
  IBufferNode * _first_buffer_node = NULL;
  struct iovec  _write_iov[_S_max_iov] = {{0, 0}};

  if (_how_much < 0)
    return -1;
  else if (_M_size == 0)
    return 0;

  _first_buffer_node = _M_buffer_list._M_first->_M_buffer_node;
  _last_buffer_node  = _M_buffer_list._M_last_with_data->_M_buffer_node;

  //__assert((_first_buffer_node != NULL && _last_buffer_node != NULL));

  _num_to_write = std::min(_M_size, (size_t)_how_much);

  if (_first_buffer_node == _last_buffer_node) {
    //__assert((_M_size == _first_buffer_node->writable_size()));
    _num_wrote = _first_buffer_node->write(_fd, _num_to_write);
  }
  else {
    _iov_len = _M_setup_write_vecs(_num_to_write, _write_iov);
    //__assert(_iov_len > 1);
    _num_wrote = writev(_fd, _write_iov, _iov_len);
  }

  if (_num_wrote > 0) {
    if (_iov_len > 0)
      _M_forward_write_ptr(_num_wrote);
    _M_size -= _num_wrote;
  }

  return _num_wrote;
}

ssize_t IEventBuffer::write(char *_buf, int _how_much)
{
  ssize_t         _num_wrote = 0;
  ssize_t         _num_to_write = 0;
  int             _iov_len = 0;
  IBufferNode *   _last_buffer_node = NULL;
  IBufferNode *   _first_buffer_node = NULL;
  struct iovec    _write_iov[_S_max_iov] = {{0, 0}};
  struct iovec *  _iov_ptr;

  if (_how_much < 0)
    return -1;
  else if (_M_size == 0)
    return 0;

  _first_buffer_node = _M_buffer_list._M_first->_M_buffer_node;
  _last_buffer_node  = _M_buffer_list._M_last_with_data->_M_buffer_node;

  //__assert((_first_buffer_node != NULL && _last_buffer_node != NULL));

  _num_to_write = std::min(_M_size, (size_t)_how_much);

  if (_first_buffer_node == _last_buffer_node) {
    //__assert((_M_size == _first_buffer_node->writable_size()));
    _num_wrote = _first_buffer_node->write(_buf, _num_to_write);
  }
  else {
    _iov_len = _M_setup_write_vecs(_num_to_write, _write_iov);
    //__assert(_iov_len > 1);
    for (int _i = 0; _i < _iov_len; ++_i) {
      _iov_ptr = _write_iov + _i;
      memcpy(_buf + _num_wrote, _iov_ptr->iov_base, _iov_ptr->iov_len);
      _num_wrote += _iov_ptr->iov_len;
    }
  }

  if (_num_wrote > 0) {
    if (_iov_len > 0)
      _M_forward_write_ptr(_num_wrote);
    _M_size -= _num_wrote;
  }

  return _num_wrote;
}

std::string IEventBuffer::toString() const
{
  std::string       _obj_str;
  IBufferListNode * _list_node;

  _list_node = _M_buffer_list._M_first;

  for (int _i = 0; _list_node != NULL; _list_node = _list_node->_M_next_node, ++_i)
    _obj_str += _list_node->_M_buffer_node->toString();

  return _obj_str;
}

IEventBuffer::~IEventBuffer()
{
  IBufferListNode *_node_to_delete;
  IBufferListNode *_list_node = _M_buffer_list._M_first;

  while (_list_node != NULL) {
    _node_to_delete = _list_node;
    _list_node = _list_node->_M_next_node;
    delete _node_to_delete;
  }
}

