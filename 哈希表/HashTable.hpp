#pragma once

#include <iostream>
#include <vector>
#include <utility>

enum State
{
	EMPTY,
	EXIST,
	DELETE
};

template<class K,class V>
struct HashDate
{ 
	pair<K, V> _kv;
	State _state;
};


template <class K,class V>
class HashTable
{
public:
	bool Insert(const pair<K, V>& kv)
	{
		//扩容
		if()



		size_t hashi = kv.first % _tables.size();//获取映射位置，应该键值的key % 数组中有效元素的个数

		//线性探测
		while (_tables[hashi].state==EXIST)//探查到目标映射位置存在数字时就进行探测
		{
			++hashi;
			hashi %= _tables.size();//取模的方式防止hashi越界
		}

		//确定好位置后进行插入
		_tables[hashi]._kv = kv;//在合适的映射位置插入
		_tables[hashi]._state = EXIST;//然后后将该位置的状态标识为EXIST
		++_n;//数组中有效元素个数+1
	}


private:
	vector<HashDate<K, V>> _tables;//向vector中存放的元素是多个HashDate类类型的对象，该对象中又包含了一个pair类类型和状态标识符
	size_t _n;//有效数据个数
};