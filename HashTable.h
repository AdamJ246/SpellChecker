
class HashTable
{
        public :
               HashTable(int NumLocs);
               void insert(string key);
	       bool find(string key);

        private :
               string* array;
	       int NumLocs;
               int fold(string key);
};

HashTable::HashTable(int size)
{
	NumLocs = size * 2;
	array = new string[NumLocs];
	for (int i = 0;  i < NumLocs;  i++)
		array[i] = "///";
}

void HashTable::insert(string key)
{
         int slot = fold(key), i;
         bool found = false;

         i = slot % NumLocs;
         while(array[i] != "///")
                i = (i + 1) % NumLocs;

         array[i] = key;
}

bool HashTable::find(string key)
{
         int slot = fold(key), i;
         bool found = false;

         i = slot % NumLocs;
         while(array[i] != "///")
	 {
		if (array[i] == key)
			return true; 
                i = (i + 1) % NumLocs;
	 }

	return false; 

}

int HashTable::fold(string key)
{
        string s;  int sum = 0;
        for (int i = 0; i < key.length();  i++)
                if (isalpha(key[i]))
                        s += tolower(key[i]);

        for (int i = 0;  i < s.length();  i++)
                sum += s[i];

        return sum;
}
