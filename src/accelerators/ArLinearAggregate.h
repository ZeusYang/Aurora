#ifndef ARLINEAREAGGREGATE_H
#define ARLINEAREAGGREGATE_H

#include "ArHitable.h"

namespace Aurora
{
	class ALinearAggregate final : public AHitableAggregate
	{
	public:
		typedef std::shared_ptr<ALinearAggregate> ptr;

		ALinearAggregate(const std::vector<AHitable::ptr> &hitables);
		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, ASurfaceInteraction &iset) const override;

		virtual ABounds3f worldBound() const override;

		virtual std::string toString() const override { return "LinearAggregate[]"; }

	private:
		using AHitableBuffer = std::vector<AHitable::ptr>;
		AHitableBuffer m_hitableList;
		ABounds3f m_worldBounds;
	};

}

#endif