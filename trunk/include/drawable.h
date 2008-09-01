#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "quaternion.h"

namespace k 
{
	class drawable2D
	{
		protected:
			vector2 mPosition;

			/**
			 * Width x Height
			 */
			vector2 mScale;

			/**
			 * Rotation in degrees
			 */
			vec_t mRotation;

		public:
			void setPosition(const vector2& pos);
			void setRotation(const vec_t rot);
			void setScale(const vector2& size);

			const vector2& getPosition();
			const vec_t getRotation();
			const vector2& getDimension();

			virtual ~drawable2D();
			virtual void draw() = 0;
	};

	class drawable3D
	{
		protected:
			vector3 mScale;
			vector3 mPosition;
			quaternion mOrientation;

		public:
			void setOrientation(const quaternion& orientation);
			void setScale(const vector3& scale);
			void setScale(const vec_t scale);
			void setPosition(const vector3& pos);

			const vector3& getScale();
			const vector3& getPosition();
			const quaternion& getOrientation();

			virtual ~drawable3D();
			virtual void draw() = 0;
	};
}

#endif

