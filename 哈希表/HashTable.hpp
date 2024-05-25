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
		//����
		if()



		size_t hashi = kv.first % _tables.size();//��ȡӳ��λ�ã�Ӧ�ü�ֵ��key % ��������ЧԪ�صĸ���

		//����̽��
		while (_tables[hashi].state==EXIST)//̽�鵽Ŀ��ӳ��λ�ô�������ʱ�ͽ���̽��
		{
			++hashi;
			hashi %= _tables.size();//ȡģ�ķ�ʽ��ֹhashiԽ��
		}

		//ȷ����λ�ú���в���
		_tables[hashi]._kv = kv;//�ں��ʵ�ӳ��λ�ò���
		_tables[hashi]._state = EXIST;//Ȼ��󽫸�λ�õ�״̬��ʶΪEXIST
		++_n;//��������ЧԪ�ظ���+1
	}


private:
	vector<HashDate<K, V>> _tables;//��vector�д�ŵ�Ԫ���Ƕ��HashDate�����͵Ķ��󣬸ö������ְ�����һ��pair�����ͺ�״̬��ʶ��
	size_t _n;//��Ч���ݸ���
};