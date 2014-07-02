#ifdef __cplusplus
extern "C" {
#endif

int addToSetImpl(const char* set_name, const char* item_value, int max_len_item_value);
int removeFromSetImpl(const char* set_name, const char* item_value, int max_len_item_value);
int	getSetItemsImpl(const char* set_name, char* list_values, int max_len, int* len, int* item_count);
int	getSetItemCountImpl(const char* set_name, int* item_count);

#ifdef __cplusplus
}
#endif

