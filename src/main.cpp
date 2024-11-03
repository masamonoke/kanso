#include "starter.hpp"

int main(int argc, char** argv) {

	kanso::starter s { argc, argv };
	s.make_app()->run();

	return 0;
}
