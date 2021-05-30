/* 6.7 Thread Safe Queue with locking and waiting */

#include<memory>
#include<mutex>
#include<condition_variable>


template<typename T>
class threadsafe_queue{
private:
	struct node{
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;
	};

	std::mutex head_mutex;
	std::unique_ptr<node> head;
	std::mutex tail_mutex;
	node* tail;
	std::condition_variable data_cond;

public:
	/* AL CONSTRUIR LA COLA, CREAMOS UN NODO DUMMY, HEAD Y TAIL APUNTAN HACIA EL  */
	threadsafe_queue():head(new node),tail(head.get()){}
	
	/* SE ELIMINAN EL CONSTRUCTOR DE COPIA Y EL DE ASIGNACION */
	threadsafe_queue(const threadsafe_queue& other)=delete;				// constructor de copia
	threadsafe_queue& operator=(const threadsafe_queue& other)=delete;		// constructor de asignacion

	std::shared_ptr<T> try_pop();
	bool try_pop(T& value);
	std::shared_ptr<T> wait_and_pop();
	void wait_and_pop(T& value);
	void push(T new_value);
	bool empty();

// 6.9 pop helper functions

	node* get_tail(){
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		return tail;
	}
	
	std::unique_ptr<node> pop_head(){
		std::unique_ptr<node> old_head = std::move(head);
		head = std::move(old_head->next);
		return old_head;
	}

	std::unique_lock<std::mutex> wait_for_data(){
		std::unique_lock<std::mutex> head_lock(head_mutex);
		data_cond.wait(head_lock, [&]{return head.get() != get_tail();});
		return std::move(head_lock);
	}

	std::unique_ptr<node> wait_pop_head(){
		std::unique_lock<std::mutex> head_lock(wait_for_data);
		return pop_head();
	}

	std::unique_ptr<node> wait_pop_head(T& value){
		std::unique_ptr<std::mutex> head_lock(wait_for_data());
		value = std::move(*head->data);
		return pop_head();
	}

// 6.10 try_pop and empty 
	
	std::unique_ptr<node> try_pop_head(){
		std::lock_guard<std::mutex> head_lock(head_mutex)	;
		if(head.get() == get_tail()){
			return std::unique_ptr<node>();
		}
		return pop_head();
	}

	std::unique_ptr<node> try_pop_head(T& value){
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if(head.get() == tail.get()){
			return std::unique_ptr<node>();
		}
		value=std::move(*head->data);
		return pop_head();
	}

};

template<typename T>
void threadsafe_queue<T>::push(T new_value){
	std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
	std::unique_ptr<node> p(new node);
	{
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		tail->data = new_data;
		node* new_tail = p.get();
		tail->next = std::move(p);
		tail = new_tail;

	}
	data_cond.notify_one();
}

// 6.9 pop usa una serie de funciones de ayuda definidas privadas

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop(){
	std::unique_ptr<node> const old_head=wait_pop_head();
	return old_head;
}

template<typename T>
void threadsafe_queue<T>::wait_and_pop(T& value){
	std::unique_ptr<node> const old_head = wait_pop_head(value);
}

// 6.10 try_pop and empty implementacion
template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop(){
	std::unique_ptr<node> old_head=try_pop_head();
	return old_head->data;
}

template<typename T>
bool threadsafe_queue<T>::try_pop(T& value){
	std::unique_ptr<node> const old_head=try_pop_head(value);
	return old_head;
}

template<typename T>
bool threadsafe_queue<T>::empty(){
	std::lock_guard<std::mutex> head_lock(head_mutex);
	return (head.get() == get_tail());
}

