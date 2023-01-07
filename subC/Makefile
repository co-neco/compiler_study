SRCS= cg.c expr.c gen.c main.c misc.c scan.c stmt.c sym.c tree.c parser.c decl.c types.c
comp1: $(SRCS)
	cc -o comp1 -g $(SRCS)

clean:
	rm -f comp1 *.o *.s out

test: comp1 tests/input20.c lib/printint.c
	./comp1 tests/input20.c
	cc -o out out.s lib/printint.c
	./out
