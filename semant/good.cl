class C {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : C {
           {
		a <- x;
		b <- y;
		self;
           }
	};
};

Class B Inherits C{

};


Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

Class A inherits B{
	c: Int;
};

Class F inherits A{
	d: Int;
};
