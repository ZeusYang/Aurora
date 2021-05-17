#ifndef ARHITABLELIST_H
#define ARHITABLELIST_H

#include "ArHitable.h"

namespace Aurora
{
	class ALinearAggregate final : public AHitableAggregate
	{
	public:
		typedef std::shared_ptr<ALinearAggregate> ptr;

		ALinearAggregate() = default;
		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, ASurfaceInteraction &iset) const override;

		virtual ABounds3f worldBound() const override;

		bool isEmpty() const { return m_hitableList.empty(); }
		void addHitable(AHitable::ptr entity);
		const std::vector<AHitable::ptr>& getHitableList() const { return m_hitableList; }

		virtual std::string toString() const override { return "LinearAggregate[]"; }

	private:
		using AHitableBuffer = std::vector<AHitable::ptr>;
		AHitableBuffer m_hitableList;
		ABounds3f m_worldBounds;
	};

}

#endif