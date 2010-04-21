#ifndef STRING_HELPER_H
#define STRING_HELPER_H 1

template <class T>
T string_conv(string str) {
	istringstream str_stream(str);
	T i;
	str_stream >> i;
	return i;
}

#endif
