#pragma once

const int WAIT_TIME_TILL_NEXT_BIRTH = 18;

class Person
{
public:
	Person() : mAge(0), mConceptionCounter(0), mConceptionJitter(0), mMale(false)
	{}

	Person(int age, bool male) : mAge(static_cast<short>(age)), mConceptionCounter(0), mConceptionJitter(0), mMale(male)
	{}

	~Person()
	{}

	int age() const { return static_cast<int>(mAge); }
	void age(int age) { mAge = static_cast<short>(age); }

	bool male() const { return mMale; }
	bool female() const { return !mMale; }

	void male(bool m) { mMale = m; }

	void conceive() { if(!mMale) ++mConceptionCounter; }
	int conceived() const { return mConceptionCounter; }

	void birth_jitter(int _j) { mConceptionJitter = _j; }

	bool pregnant()
	{
		return mConceptionCounter > 0 && mConceptionCounter <= WAIT_TIME_TILL_NEXT_BIRTH + mConceptionJitter;
	}

	void update()
	{
		increment_age();

		// Assumption: This will only ever be non-zero if the Person is female.
		if (mConceptionCounter > 0)
			++mConceptionCounter;
		if (mConceptionCounter > WAIT_TIME_TILL_NEXT_BIRTH + mConceptionJitter)
			mConceptionCounter = 0;
	}

protected:

	void increment_age() { ++mAge; }
	
private:

	int		mAge;

	int		mConceptionCounter;
	int		mConceptionJitter;

	bool	mMale;
};