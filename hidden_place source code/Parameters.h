#pragma once
#include <map>
#include <string>
#include <typeindex>

class Parameters {
	std::map<std::type_index, void*> data = {};
public:
	template <typename T>
		Parameters& Set(T input) {
			data[typeid(T)] = new T(input);
			return *this;
	};

	template <typename T>
	T Get() {
		std::type_index key = std::type_index(typeid(T));
		if (data.count(key) > 0) {
			T output = *((T*)data[key]);
			data.erase(key);
			return output;
		}
		return nullptr;
	};

	~Parameters() {
		/*for (auto &item : data) {
			data.erase(item.first);
		}*/
	};
};