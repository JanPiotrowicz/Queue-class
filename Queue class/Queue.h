#pragma once

template<typename QueueType>
struct QueueIterator
{
public:
	using ValueType = typename QueueType::ValueType;
	using PointerType = ValueType*;
	using ReferenceType = ValueType&;

public:
	QueueIterator(PointerType ptr)
		: m_Ptr(ptr) {}

	QueueIterator& operator++()
	{
		m_Ptr++;
		return *this;
	}

	QueueIterator& operator++(int)
	{
		QueueIterator iterator = *this;
		(*this)++;
		return iterator;
	}

	QueueIterator& operator--()
	{
		m_Ptr--;
		return *this;
	}

	QueueIterator& operator--(int)
	{
		QueueIterator iterator = *this;
		(*this)--;
		return iterator;
	}

	ReferenceType operator[](size_t index)
	{
		return *(m_Ptr + index);
	}

	PointerType operator->()
	{
		return m_Ptr;
	}

	ReferenceType operator*()
	{
		return *m_Ptr;
	}

	bool operator==(const QueueIterator& other) const
	{
		return m_Ptr == other.m_Ptr;
	}

	bool operator!=(const QueueIterator& other) const
	{
		return !(*this == other);
	}

private:
	PointerType m_Ptr;
};

template<typename T>
struct Queue
{
public:
	using ValueType = T;
	using Iterator = QueueIterator<Queue<T>>;

public:
	void push(T& elem)
	{
		ReAllocIfNeeded();

		new(&m_Data[m_Size]) T(elem);
		m_Size++;
	}

	void push(const T& elem)
	{
		ReAllocIfNeeded();

		new(&m_Data[m_Size]) T(elem);
		m_Size++;
	}

	void push(T&& elem) noexcept
	{
		ReAllocIfNeeded();

		new(&m_Data[m_Size]) T(std::move(elem));
		m_Size++;
	}

	template<typename... Args>
	void emplace(Args&&... args)
	{
		ReAllocIfNeeded();

		new(&m_Data[m_Size]) T(std::forward<Args>(args)...);
		m_Size++;
	}

	template<typename... Args>
	void push_many(Args&&... args)
	{
		size_t n = sizeof...(args);
		ReAllocIfNeeded(n);
		((void)emplaceNoReAlloc(std::move(args)), ...);
	}

	//template<typename... Args>
	//void emplace_many(Args&&... args)		// todo
	//{
	//	
	//}

	void pop()
	{
		m_Data[0].~T();
		m_Data++;
		m_Size--;
	}

	size_t size() const
	{
		return m_Size;
	}

	bool empty() const
	{
		return m_Size == 0 ? true : false;
	}

	T& front()
	{
		return m_Data[0];
	}

	const T& front() const
	{
		return m_Data[0];
	}

	T& back()
	{
		return m_Data[m_Size - 1];
	}

	const T& back() const
	{
		return m_Data[m_Size - 1];
	}

	void print() const
	{
		for (size_t i = 0; i < m_Size; i++)
			std::cout << m_Data[i] << '\n';
	}

	std::ostream& print(std::ostream& stream) const
	{
		for (size_t i = 0; i < m_Size; i++)
			stream << m_Data[i] << '\n';
		return stream;
	}

	void clear()
	{
		for (size_t i = 0; i < m_Size; i++)
			m_Data[i].~T();
		m_Size = 0;
		m_Data = m_Memory;
	}

	void set(T& value)
	{
		for (size_t i = 0; i < m_Size; i++)
			m_Data[i] = value;
	}

	void set(const T& value)
	{
		for (size_t i = 0; i < m_Size; i++)
			m_Data[i] = value;
	}

	template<typename... Args>
	void set(Args&&... args)
	{
		T value(args...);

		for (size_t i = 0; i < m_Size; i++)
			m_Data[i] = value;
	}

	T& operator[](const size_t index)
	{
		return m_Data[index];
	}

	const T& operator[](const size_t index) const
	{
		return m_Data[index];
	}

	Iterator begin() const
	{
		return m_Data;
	}

	Iterator end() const
	{
		return m_Data + m_Size;
	}

	Queue()
	{
#if DBG_Queue
		std::cerr << "Constructing() Queue!\n";
#endif
		ReAlloc(2);
	}

	Queue(std::initializer_list<T> init_list)
	{
#if DBG_Queue
		std::cerr << "Constructing() Queue!\n";
#endif
		ReAlloc(2 * init_list.size());
		for (auto& it : init_list)
			push(std::move(it));
	}

	Queue(Queue& other)
	{
#if DBG_Queue
		std::cerr << "Copying() Queue\n";
#endif
		m_Capacity = 2 * other.m_Size;
		m_Memory = (T*)::operator new(m_Capacity * sizeof(T));
		m_Data = m_Memory;
		m_Size = other.m_Size;

		for (size_t i = 0; i < other.m_Size; i++)
			m_Data[i] = other.m_Data[i];
	}

	Queue(const Queue& other)
	{
#if DBG_Queue
		std::cerr << "Copying() Queue\n";
#endif
		m_Capacity = 2 * other.m_Size;
		m_Memory = (T*)::operator new(m_Capacity * sizeof(T));
		m_Data = m_Memory;
		m_Size = other.m_Size;

		for (size_t i = 0; i < other.m_Size; i++)
			m_Data[i] = other.m_Data[i];
	}

	Queue& operator=(Queue& other)
	{
#if DBG_Queue
		std::cerr << "Copying= Queue\n";
#endif
		for (size_t i = 0; i < m_Size; i++)
			m_Data[i].~T();

		::operator delete(m_Memory, m_Capacity * sizeof(T));

		m_Capacity = 2 * other.m_Size;
		m_Memory = (T*)::operator new(m_Capacity * sizeof(T));
		m_Data = m_Memory;
		m_Size = other.m_Size;

		for (size_t i = 0; i < other.m_Size; i++)
			m_Data[i] = other.m_Data[i];

		return *this;
	}

	Queue& operator=(const Queue& other)
	{
#if DBG_Queue
		std::cerr << "Copying= Queue\n";
#endif
		for (size_t i = 0; i < m_Size; i++)
			m_Data[i].~T();

		::operator delete(m_Memory, m_Capacity * sizeof(T));

		m_Capacity = 2 * other.m_Size;
		m_Memory = (T*)::operator new(m_Capacity * sizeof(T));
		m_Data = m_Memory;
		m_Size = other.m_Size;

		for (size_t i = 0; i < other.m_Size; i++)
			m_Data[i] = other.m_Data[i];

		return *this;
	}

	Queue(Queue&& other) noexcept
	{
#if DBG_Queue
		std::cerr << "Moving() Queue\n";
#endif
		m_Memory = other.m_Memory;
		other.m_Memory = nullptr;
		m_Capacity = other.m_Capacity;
		m_Data = m_Memory;
		m_Size = other.m_Size;
	}

	Queue& operator=(Queue&& other)	noexcept
	{
#if DBG_Queue
		std::cerr << "Moving= Queue\n";
#endif
		m_Memory = other.m_Memory;
		other.m_Memory = nullptr;
		m_Capacity = other.m_Capacity;
		m_Data = m_Memory;
		m_Size = other.m_Size;

		return *this;
	}

	~Queue()
	{
#if DBG_Queue
		std::cerr << "Deleting() Queue\n";
#endif
		for (size_t i = 0; i < m_Size; i++)
			m_Data[i].~T();

		::operator delete(m_Memory, m_Capacity * sizeof(T));
	}

	friend std::ostream& operator<<(std::ostream& os, const Queue<T>& other)
	{
		for (size_t i = 0; i < other.m_Size; i++)
		{
			if (i == 0)
			{
				os << other.m_Data[i];
				continue;
			}
			os << ' ' << other.m_Data[i];
		}

		return os;
	}

	void reserve(size_t size)
	{
		ReAlloc(size);
	}

private:
	void ReAlloc(size_t newCapacity) noexcept
	{
#if DBG_Queue
		std::cerr << "ReAllocing Queue to: " << newCapacity << " objects\n";
#endif
		T* newBlock = (T*)::operator new(newCapacity * sizeof(T));

		for (size_t i = 0; i < std::min(m_Size, newCapacity); i++)
			newBlock[i] = std::move(m_Data[i]);

		for (size_t i = 0; i < m_Size; i++)
			m_Data[i].~T();

		::operator delete(m_Memory, m_Capacity * sizeof(T));

		m_Memory = newBlock;
		m_Data = m_Memory;
		m_Capacity = newCapacity;
		m_Size = std::min(m_Size, newCapacity);
	}

	void ReAllocIfNeeded()
	{
		if (m_Data + m_Size >= m_Memory + m_Capacity)
			ReAlloc(2 * m_Size + 1);
	}

	void ReAllocIfNeeded(size_t numberOfElems)
	{
		if (m_Data + m_Size + numberOfElems >= m_Memory + m_Capacity)
			ReAlloc(2 * m_Size + numberOfElems);
	}

	void emplaceNoReAlloc(T&& obj)
	{
		new(&m_Data[m_Size]) T(std::move(obj));
		m_Size++;
	}

private:
	T* m_Memory = nullptr;
	size_t m_Capacity = 0;
	T* m_Data = nullptr;
	size_t m_Size = 0;
};
