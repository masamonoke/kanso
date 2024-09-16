#pragma once

#include <exception>
#include <string>

namespace kanso::exception {

	class base_kanso_exception : public std::exception {
		public:
			base_kanso_exception(std::string&& msg) : e(msg) {} // NOLINT(bugprone-throw-keyword-missing)

			[[nodiscard]] const char* what() const noexcept override {
				return e.what();
			}

		private:
			std::runtime_error e;
	};

} // namespace kanso::exception
