#ifndef STRING_HELPER_H
#define STRING_HELPER_H 1

template <class T>
T string_conv(std::string str) {
	std::istringstream str_stream(str);
	T i;
	str_stream >> i;
	return i;
};

template <class T>
std::string conv_string(T i) {
	std::stringstream str;
	str << i;
	return str.str();
};

template<class T>
inline T string_trim(T str, std::string trim_chars = " \t\n");

template<>
inline std::string string_trim(std::string str, std::string trim_chars)
 {
	size_t start_pos = str.find_first_not_of(trim_chars);
	
	size_t end_pos = str.find_last_not_of(trim_chars);
	
	if ((start_pos == string::npos) || (end_pos == string::npos)) {
		return "";
	}
	
	return str.substr(start_pos, end_pos-start_pos+1);
};

#endif
