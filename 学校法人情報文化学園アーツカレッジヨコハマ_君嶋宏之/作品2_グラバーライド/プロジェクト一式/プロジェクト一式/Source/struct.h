#pragma once
/*

	template<typename T>
	struct Vector2
	{
		T x, y;
		Vector2() : x(T()), y(T()) {}
		Vector2(T _s) : x(_s), y(_s) {}//“¯ˆê‚Ìsame
		Vector2(T _x, T _y) : x(_x), y(_y) {}
		template <typename U>
		Vector2(Vector2<U> _v) : x((T)_v.x), y((T)_v.y) {}
		//“¯‚¶Œ^‚ğ‘ã“ü
		inline const Vector2<T>& operator=(const Vector2<T>& _other)
		{
			x = _other.x;
			y = _other.y;
			return *this;
		}
		//•Ê‚ÌŒ^‚ğƒLƒƒƒXƒg‚µ‚Ä‘ã“ü
		template <typename U>
		inline const Vector2<T>& operator=(const Vector2<U>& _other)
		{
			x = (T)_other.x;
			y = (T)_other.y;
			return *this;
		}
		//ˆê‚Â‚Ì’l‚ğx,y—¼•û‚É‘ã“ü
		template <typename U>
		inline const Vector2<T>& operator=(const U& _other)
		{
			x = (T)_other;
			y = (T)_other;
			return *this;
		}
		//À•W‚©‚çŠp“x‚ğZo
		inline T angle() const {
			return atan2f((float)y, (float)x);
		}
		//ƒxƒNƒgƒ‹“¯m‚ÌƒTƒCƒY‚ğZo
		template <typename U>
		inline T V2Size(const Vector2<U>& _other) const {
			return sqrt(x * _other.x + y * _other.y);
		}

		inline bool operator==(const Vector2<T>& _b)
		{
			bool ret{};
			ret = (x == _b.x) && (y == _b.y);
			return ret;
		}
		inline Vector2<T> operator+(const Vector2<T>& _b)
		{
			Vector2<T> ret{};
			ret.x = x + _b.x;
			ret.y = y + _b.y;
			return ret;
		}
		
		inline const void operator+=(const Vector2<T>& _b)
		{
			x += _b.x;
			y += _b.y;
		}
	
		inline Vector2<T> operator-( const Vector2<T>& _b)
		{
			Vector2<T> ret{};
			ret.x = x - _b.x;
			ret.y = y - _b.y;
			return ret;
		}
		inline const void operator-=( const Vector2<T>& _b)
		{
			x -= _b.x;
			y -= _b.y;
			
		}
		
		inline Vector2<T> operator*(const T& _other)
		{
			Vector2<T> ret{};
			ret.x = x * _other;
			ret.y = y * _other;
			return ret;
		}
		inline void operator*=(const T& _other)
		{
			x *= _other;
			y *= _other;
		}
		inline Vector2<T> operator/( const T& _other)
		{
			Vector2<T> ret{};
			ret.x = x / _other;
			ret.y = y / _other;
			return ret;
		}
		
		inline Vector2<T> operator/(const Vector2<T>& _b)
		{
			Vector2<T> ret{};
			ret.x = x / _b.x;
			ret.y = y / _b.y;
			return ret;
		}
		
		inline Vector2<T>& operator/=(const T& _other)
		{
			x /= _other;
			y /= _other;
		}
		

		inline T GetV2Angle(const T& _x, const T& _y) {
			return atan2f((float)_y, (float)_x);
		}

		// ³‹K‰»iNormalizej
		inline Vector2<T> Normalize(const Vector2<T>& vec) {
			T length = sqrt(vec.x * vec.x + vec.y * vec.y);
			if (length == 0) return Vector2<T>(0, 0); // ƒ[ƒƒxƒNƒgƒ‹‚Í‚»‚Ì‚Ü‚Ü
			return Vector2<T>(vec.x / length, vec.y / length);
		}
		// ‹——£ŒvZiDistancej
		inline T Distance(const Vector2<T>& a, const Vector2<T>& b) {
			T dx = a.x - b.x;
			T dy = a.y - b.y;
			return sqrt(dx * dx + dy * dy);
		}
		// ‰ñ“]iRotatej
		inline Vector2<T> Rotate(const Vector2<T>& vec, T angle) {
			T cosA = cos(angle);
			T sinA = sin(angle);
			return Vector2<T>(
				vec.x * cosA - vec.y * sinA,
				vec.x * sinA + vec.y * cosA
			);
		}
		inline const Vector2<T> GetV2ZERO() {
			return Vector2(T(0), (T)(0));
		} 

	
	};		
	//“ü‚ê‚½’l‚ğVector2Œ^‚Åreturn
	template<typename T>
	inline Vector2<T> V2Get(const T& _x, const T& _y) {
		return Vector2<T>(_x, _y);
	}
	template<typename T>
	//“ü‚ê‚½’l‚ğVector2*Œ^‚Åreturn
	inline Vector2<T>* V2PGet(T _x, T _y) {
		return new Vector2<T>(_x, _y);
	}

	//intŒ^vector2
	typedef Vector2<int> Vector2I;
	//floatŒ^vector2
	typedef Vector2<float> Vector2F;
	//doubleŒ^vector2
	typedef Vector2<double> Vector2D;


*/

struct HitStopState
{
	float power;
	int time;
};

inline HitStopState HitStopGet(float _power, int _time) {

	HitStopState result;
	result.power = _power;
	result.time = _time;
	return result;
}

typedef HitStopState ShakeState;

inline ShakeState ShakeGet(float _power, int _time) {
	return HitStopGet(_power, _time);
}

struct ControlVibration
{
	int power;
	int time;
};

inline ControlVibration VibrationGet(int _power, int _time) {

	ControlVibration result;
	result.power = _power;
	result.time = _time;
	return result;
}

struct GravityMoveMax
{
	float normalGravityAddMax;
	float playerChangeGravityAddMax;
};

inline GravityMoveMax MaxGet(float normal, float playerChange) {
	GravityMoveMax result;
	result.normalGravityAddMax = normal;
	result.playerChangeGravityAddMax = playerChange;
	return result;
}





