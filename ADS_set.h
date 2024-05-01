#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 7>
class ADS_set {
public:
  class Iterator;
  using value_type = Key;
  using key_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using const_iterator = Iterator;
  using iterator = const_iterator;
  using key_equal = std::equal_to<key_type>;
  using hasher = std::hash<key_type>;
  
private:
	struct Element;
	size_type c_size{0};
	size_type t_size{N};
	Element **table;
	void add(const key_type &key);
	size_type hash(const key_type &key) const;
	void rehash();
public:

  ADS_set() : c_size(0), t_size(N) {
  	table = new Element*[t_size];
  	for(size_type i{0}; i < t_size; i++) {
  		table[i] = nullptr;
  	}
  }
  ADS_set(std::initializer_list<key_type> ilist) : ADS_set{} {insert(ilist);}
  template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{} {insert(first, last);}
  
  ADS_set(const ADS_set &other) : ADS_set() {
  	for(const auto &i : other) {
  		add(i);
  	}
  }

  ~ADS_set() {
  	for(size_type i{0}; i < t_size; i++) {
  		delete table[i];
  		 }
  	delete[] table;
  }

  ADS_set &operator=(const ADS_set &other);
  ADS_set &operator=(std::initializer_list<key_type> ilist);

  size_type size() const {return c_size;}
  bool empty() const {return c_size==0;}

  void insert(std::initializer_list<key_type> ilist);
  std::pair<iterator,bool> insert(const key_type &key); 
  template<typename InputIt> void insert(InputIt first, InputIt last);

  void clear();
  size_type erase(const key_type &key); 

  size_type count(const key_type &key) const;                          
  iterator find(const key_type &key) const;

  void swap(ADS_set &other);

  const_iterator begin() const;
  const_iterator end() const;

  void dump(std::ostream &o = std::cerr) const;

  friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) {
  
  	if(lhs.c_size != rhs.c_size) {
  		return false;
  	}
  	
		for(const auto &k : lhs) {
			if(rhs.count(k) == 0){
				return false;
			}
		}	
		
  	return true;
  }
  
  friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) {return !(lhs==rhs);}
  
};

template<typename Key, size_t N>
struct ADS_set<Key,N>::Element {
	key_type key;
	Element *next;
	
	Element(key_type k) : key(k), next(nullptr) {}
	Element() : key(0), next(nullptr) {}
	Element(Element const &e) : key(e.key), next(e.next) {}
	~Element() {
		if (next != nullptr) {
			delete next;
		 }
		}	
};

template<typename Key, size_t N>
template<typename InputIt> void ADS_set<Key, N>::insert(InputIt first, InputIt last) {
	for(auto it{first}; it != last; ++it) {
		add(*it);
	}

}

template<typename Key, size_t N>
void ADS_set<Key, N>::insert(std::initializer_list<key_type> ilist) {
	insert(ilist.begin(), ilist.end());
} 


template<typename Key, size_t N>
std::pair<typename ADS_set<Key,N>::iterator,bool> ADS_set<Key, N>::insert(const key_type &key) {
	
	if(count(key) == 1) {
		return std::make_pair(find(key), false);
	}
	
	add(key);
	return std::make_pair(find(key), true);

}

template <typename Key, size_t N>
void ADS_set<Key, N>::add(const key_type &key) {
	if(t_size!=0 ) {
	
	if(((double)c_size/(double)t_size)>=0.7)
			rehash();
	
	if(count(key) == 0) {
		c_size++;
		size_type ind = hash(key);
		Element *e = new Element(key);
		
		if(table[ind] == nullptr) {
			table[ind] = e;
		}
		else {
			Element *current = table[ind];
			table[ind] = e;
			e->next = current;
		}
	}	
	}
}


template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::hash(const key_type &key) const{  
	return hasher{}(key) % t_size;
}

template <typename Key, size_t N>
void ADS_set<Key, N>::rehash() {
	size_type new_ts = t_size*2+1;
	Element **new_table{new Element*[new_ts]};
	
	for(size_type n{0}; n < new_ts; n++) {
		new_table[n] = nullptr;
	}
	
	for(size_type i{0}; i< t_size; i++) {
		Element *o_current = table[i];
		Element *o_next = nullptr;
		
		
		while(o_current != nullptr) {
		size_type ni =  {hasher{}(o_current->key)%new_ts};
			o_next = o_current->next;
			
			if(new_table[ni] == nullptr) {
				new_table[ni] = o_current;
				new_table[ni]->next = nullptr;
			}
			else {
				Element *help = new_table[ni];
				new_table[ni] = o_current;
				new_table[ni]->next = help;
			}
		o_current = o_next;
		}
	} 
	
	delete[] table;
	t_size = new_ts;
	this->table = new_table;
	
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key,N>::count(const key_type &key) const {
	size_type index = hash(key);
	
	Element *current = table[index];
	
	if(current == nullptr) {
		return 0;
	}
	
	do { 
		if(key_equal{}(current->key, key)) {
			return 1;
		}
		else {
			current = current->next;
		}
	} 
	while(current != nullptr);

	return 0;

	
}
template<typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream &o) const {
	o<<"table size: " << t_size << " current size: " << c_size << std::endl;
	
	for(size_type i{0}; i< t_size; ++i) {
		o<< i;
		Element *e = table[i];
		while( e != nullptr) {
			o<< "->" << e->key;
			e = e->next;
		}
		o<<std::endl;
	  }
	o<<std::endl;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set &other) {
	using std::swap;
	swap(table, other.table);
	swap(c_size, other.c_size);
	swap(t_size, other.t_size);
}


template<typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(std::initializer_list<key_type> ilist) {
	ADS_set new_set{ilist};  
	swap(new_set);
	return *this;
}


template<typename Key, size_t N>
void ADS_set<Key, N>::clear() {
	ADS_set empty_set;
	swap(empty_set);
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const key_type &key) {
	
	
	if(count(key) == 0) { 
		return 0;
	}
	
	size_type index{hash(key)};
	
	Element *current = table[index];
	Element *previous = nullptr;
	
	if(key_equal{}(current->key, key)) {
		--c_size;
		table[index] = table[index]->next;
		current->next=nullptr;
		delete current;
		return 1;
	}
	
	while(current->next != nullptr) {
		previous = current;
		current = current->next;
		
		if(key_equal{}(current->key, key)) {
			previous->next = current->next;
			current->next=nullptr;
			delete current;
			--c_size;
			return 1;
		}
	
	}
	delete current;
	delete previous;
		
return 1;

}

template<typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(const ADS_set &other) {
	ADS_set ads_{other};
	swap(ads_);
	return *this;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type &key) const {
		
		if(count(key) == 0) return end();
		 
 	size_type index{hash(key)};
 	Element* help = table[index];
 	while(help!=nullptr) {
 		if(key_equal{}(help->key,key)){
 		auto it = Iterator{table, help, t_size, index};
 			return it;
 		}
 		else {
 			help = help->next;
 		}
 	
 	
 	}
 	return end();
}


template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::begin() const {
	return Iterator{table, t_size};
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::end() const {
	return Iterator{table, t_size, t_size};
}


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
public:
  using value_type = Key;
  using difference_type = std::ptrdiff_t;
  using reference = const value_type&;
  using pointer = const value_type*;
  using iterator_category = std::forward_iterator_tag;

private:
	Element **table; 
	Element *current_element; 
	size_type table_index;
	size_type table_size;
	bool speshul{false};
	// counter
	
	void skip() {
	
		if(table_index == table_size) {
  		current_element = nullptr;
  		return;
  	}
  	
  	if(current_element != nullptr ) {
			if(current_element->next != nullptr) {
				current_element = current_element->next;
				return;
				}
			else {
				table_index++;
				if(table_index == table_size) {
  				current_element = nullptr;
  				return;
  		 }
			}
  	}
  	
  	while(table[table_index] == nullptr ) {
  		table_index++;	
  		
  		if(table_index == table_size) {
				current_element = nullptr;
				return;
  		 }
  	}
  	
  	current_element = table[table_index];
	
	}

public:

  Iterator(Element **table, size_type table_size) : table {table}, current_element{nullptr}, table_index{0}, table_size{table_size}   { 
  	skip();
  }
  
	explicit Iterator() : table{nullptr}, current_element{nullptr}, table_index{0}, table_size{0} {}
	
  Iterator(Element **table, size_type table_size, size_type table_index) : table {table},current_element{nullptr}, table_index{table_index}, table_size{table_size}  { 
  	skip();
  }
	
	Iterator(Element **table, Element *current_element, size_type table_size, size_type table_index) : table{table}, current_element{current_element},  table_index{table_index}, table_size{table_size}{}
	
  reference operator*() const { return current_element->key; }

  pointer operator->() const { return &current_element->key; }

  Iterator &operator++() { 
		skip();
		return *this;  	
  }
  
  Iterator operator++(int) { 
  	auto old {*this};
  	++*this;
  	return old;
  }
  
  friend bool operator==(const Iterator &lhs, const Iterator &rhs) { return lhs.current_element == rhs.current_element; }  // key_equal{}(current->key, key) ???????????? 
  
  friend bool operator!=(const Iterator &lhs, const Iterator &rhs) { return !(lhs == rhs); }

};

template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }
#endif