#pragma once

#include "app.hpp"

namespace kanso {

	class starter {
		public:
			starter(int argc, char** argv);

			[[nodiscard]] std::unique_ptr<app> make_app();

		private:
			int argc_;
			char** argv_;
			bool init_ = false;
	};

};
