#include "object_manager.hpp"
#include "light.hpp"
#include "model.hpp"

namespace kanso {

	object_manager::object_manager(std::vector<std::shared_ptr<model>> models, std::vector<std::shared_ptr<light>> lights)
	    : models_(std::move(models)),
	      lights_(std::move(lights))
	{
		model_views_.reserve(models_.size());
		for (const auto& model : models_) {
			model_view const view{
				model->id(),
				model->name(),
				{ model->pos()[0], model->pos()[1], model->pos()[2] },
				{ model->rot()[0], model->pos()[1], model->pos()[2] },
				{ model->scale()[0], model->scale()[1], model->scale()[2] },
				model->type()
			};
			model_views_.emplace_back(view);
		}

	}

	void object_manager::add_model(std::unique_ptr<model> model) {
		model_view const view{
			model->id(),
			model->name(),
			{ model->pos()[0], model->pos()[1], model->pos()[2] },
			{ model->rot()[0], model->pos()[1], model->pos()[2] },
			{ model->scale()[0], model->scale()[1], model->scale()[2] },
			model->type()
		};
		model_views_.emplace_back(view);
		models_.emplace_back(std::move(model));
	}
}
