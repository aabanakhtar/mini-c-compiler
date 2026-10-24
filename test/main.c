void printX() {
	printf("X!");
}

int add(int a, int b) { 
	return b + a; 
}

int main() { 
	printX();
	void x = 5;
	int boot = add(3, 4); 
	if (boot <= 7 && boot > 0) {
		printf("Boot is between 7 and 0!");
	} else {
		printf("Boot is not between 7 and 0!");
	}

	printX();
	return 0;
}