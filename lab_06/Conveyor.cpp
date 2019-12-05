#include "stdafx.h"
#include "windows.h"
#include <mutex>
#include <thread>
#include <queue>
#include <iostream>


using Ms = std::chrono::milliseconds;

class Unit {};

class Component {};

/*
unsigned long long tick(void) {
	unsigned long long d;
	__asm __volatile__("rdtsc" : "=A"(d));
	return d;
}
*/

unsigned __int64 tick() 
{ 
	return (__rdtsc()); 
}


class Body : public Component
{
public:
	Body() { built = false; }

	void build() { Body::count++; built = true; }

	bool is_built() { return built; }

	static int count;
protected:
	bool built;

	int max_health;
	int health;
	int rotation_speed;
	int current_rotation_angle;
};

class Engine : public Component
{
public:
	Engine() { built = false; }

	void build() { Engine::count++; built = true; }

	int get_speed() const;
	int get_max_speed() const;
	int get_max_backspeed() const;

	void get_speed(int sp);
	void get_max_speed(int sp);
	void get_max_backspeed(int sp);

	static int count;

	bool is_built() { return built; }
private:
	bool built;
	int speed_;
	int max_speed_;
	int max_backspeed_;
};

class Electronics : public Component
{
public:
	Electronics() { built = false; }
	void build() { Electronics::count++; built = true; }

	bool is_built() { return built; }

	void fire();

	static int count;
private:
	bool built;
	int damage;
	int recharge;
	int amount_bullets;
};

class Car : public Unit
{
public:
	static int count;
	int id;
	std::mutex Car_mutex;

	Car() { id = Car::count++; built = false; }
	~Car() {}

	void insert_Electronics(Electronics Electronics, DWORD time) {
		Sleep(time);
		_Electronics = Electronics;
	}

	void insert_body(Body body, DWORD time) {
		Sleep(time);
		_body = body;
	}

	void insert_engine(Engine engine, DWORD time) {
		Sleep(time);
		_engine = engine;
	}

	bool has_engine() { return _engine.is_built(); }
	bool has_Electronics() { return _Electronics.is_built(); }
	bool has_body() { return _body.is_built(); }

	bool is_ready() { return _engine.is_built() && _body.is_built() && _Electronics.is_built(); }

	bool is_built() { return built; }


	void complete() {
		if (_engine.is_built() && _body.is_built() && _Electronics.is_built()) {
			Sleep(50);
			built = true;
		}
	}

	// Конструктор копирования для queue.pop
	Car(const Car & other) {
		_engine = other._engine;
		_body = other._body;
		_Electronics = other._Electronics;
		id = other.id;
	}

	Car & operator=(const Car & other) {
		_engine = other._engine;
		_body = other._body;
		_Electronics = other._Electronics;
		id = other.id;

		return *this;
	}

	void move(int speed);
	void rotate(double angle);
	void fire();

private:
	bool built;
	Engine _engine;
	Body _body;
	Electronics _Electronics;
};

// Для очередей, которые ничего не должны строить
Car noWork(Car t, DWORD d) { return t; };

class CarQueue {
private:
	// Очередь готовых на выполнение
	std::queue<Car> Cars;
	// Функтор - функция постройки
	std::function<Car(Car &Car, DWORD time)> work;
	// Мьютекс для блокировки доступа к очереди выполнения
	std::timed_mutex queue_mutex;
	// Мьютекс для блокировки доступа к флагу finish
	std::mutex finish_mutex;
	// Мьютекс для блокировки доступа к флагу main
	std::mutex main_mutex;

	// флаг первого рабочего конвейера
	bool main;
public:
	// флаг конца работы, можно было сделать без него, но так понятнее
	bool finish;
	// Время ожидания на мьютексе
	DWORD time_for_mutex;
	// Время ожидания появления элементов в очереди
	DWORD downtime;
	// время постройки - занесено сюда для тестирования
	DWORD build_time;
	// идентификатор машины - занесен сюда для отладки
	int id;
	// статическая переменаня нужна для id
	static int count;
	std::vector<clock_t> times_in;
	std::vector<clock_t> times_out;
	clock_t begin;
	clock_t end;

	// Конструкторы копирования и перемещения
	CarQueue(const CarQueue & other);

	CarQueue(const CarQueue && other);

	// Для очереди функций работы с машинами
	CarQueue(std::function<Car(Car&, DWORD)> f, DWORD t);

	// Для очереди готовых машин
	CarQueue() {} // не использовать больше нигде

				   // Для очереди пустых машин
	CarQueue(size_t amount);

	void set_main() {
		main = true;
	}

	void start(CarQueue &CQ);

	Car pop() {
		Car Car = Cars.front();
		Cars.pop();
		return Car;
	}

	void push(Car &Car) {
		Cars.push(Car);
	}

	size_t size() {
		return Cars.size();
	}
};

// Для пустой очереди
CarQueue::CarQueue(size_t amount) {
	end = begin = clock();
	id = CarQueue::count++;
	for (size_t i = 0; i < amount; i++) {
		Car Car;
		Cars.push(Car);
	}
	main = 1;
	finish = 0;
	work = noWork;
	time_for_mutex = 100;
	downtime = 100;
}

// Для очереди функций работы с машинами
CarQueue::CarQueue(std::function<Car(Car&, DWORD)> f, DWORD t) {
	end = begin = clock();
	id = CarQueue::count++;
	main = 0;
	finish = 0;
	work = f;
	build_time = t;
	time_for_mutex = 100;
	downtime = 100;
}

CarQueue::CarQueue(const CarQueue & other) {
	id = other.id;
	Cars = other.Cars;
	work = other.work;
	finish = other.finish;
	main = other.main;
	time_for_mutex = other.time_for_mutex;
	downtime = other.downtime;
}

CarQueue::CarQueue(const CarQueue && other) {

	id = other.id;
	Cars = other.Cars;
	work = other.work;
	finish = other.finish;
	main = other.main;
	time_for_mutex = other.time_for_mutex;
	downtime = other.downtime;
}

void CarQueue::start(CarQueue &CQ) {
	finish_mutex.lock();

	while (!finish) {
		finish_mutex.unlock();
		queue_mutex.lock();
		while (Cars.size() > 0) {
			times_in.push_back(clock());
			Car Car = pop();
			queue_mutex.unlock();
			Car = work(Car, build_time);
			bool connect = 0;
			while (!connect) {
				if (CQ.queue_mutex.try_lock_for(
					Ms(time_for_mutex))) {
					CQ.push(Car);
					times_out.push_back(clock());
					connect = 1;
					CQ.queue_mutex.unlock();
				}
				std::this_thread::sleep_for(Ms(50));
			}
			queue_mutex.lock();
		}
		queue_mutex.unlock();
		main_mutex.lock();
		if (main && Cars.size() == 0) {
			main_mutex.unlock();

			finish_mutex.lock();
			finish = 1;
			finish_mutex.unlock();

			CQ.main_mutex.lock();
			CQ.set_main();
			CQ.main_mutex.unlock();


			end = clock();
		}
		else {
			main_mutex.unlock();
			std::this_thread::sleep_for(Ms(downtime));
		}
		finish_mutex.lock();
	}
	std::cout << "\nfinish";
	finish_mutex.unlock();
}

Car add_engine(Car &Car, DWORD time) {
	Car.Car_mutex.lock();

	if (!Car.has_engine()) {
		Engine engine;
		Car.insert_engine(engine, time);
		std::cout << "\n add engine to Car " << Car.id;
	}

	Car.Car_mutex.unlock();

	return Car;
}

Car add_Electronics(Car &Car, DWORD time) {
	Car.Car_mutex.lock();

	if (!Car.has_Electronics()) {
		Electronics Electronics;
		Car.insert_Electronics(Electronics, time);
		std::cout << "\n add Electronics to Car " << Car.id;
	}

	Car.Car_mutex.unlock();

	return Car;
}

Car add_body(Car &Car, DWORD time) {
	Car.Car_mutex.lock();

	if (!Car.has_body()) {
		Body body;
		Car.insert_body(body, time);
		std::cout << "\n add body to Car " << Car.id;
	}
	Car.Car_mutex.unlock();
	return Car;
}

int Body::count = 1;
int Car::count = 1;
int Electronics::count = 1;
int Engine::count = 1;
int CarQueue::count = 1;

void test(size_t amount, int time, DWORD a, DWORD b, DWORD c) {
	unsigned long long tb, te, t_mid = 1;

	CarQueue Cars(amount);

	CarQueue queue_body(add_body, a);
	CarQueue queue_engine(add_engine, b);
	CarQueue queue_Electronics(add_Electronics, c);

	CarQueue ready_Cars;

	std::thread give_to_body([&Cars, &queue_body]() { Cars.start(queue_body); });

	std::thread threads[100];
	int j = 0;
	for (int i = 0; i < time; i++) {
		threads[j] = std::thread([&queue_body, &queue_engine]() { queue_body.start(queue_engine); });
		j++;
	}
	for (int i = 0; i < time; i++) {
		threads[j] = std::thread([&queue_engine, &queue_Electronics]() { queue_engine.start(queue_Electronics); });
		j++;
	}
	for (int i = 0; i < time; i++) {
		threads[j] = std::thread([&queue_Electronics, &ready_Cars]() { queue_Electronics.start(ready_Cars); });
		j++;
	}

	give_to_body.detach();
	for (int i = 0; i < time; i++) {
		j--;
		threads[j].join();
	}

	for (int i = 0; i < time; i++) {
		j--;
		threads[j].join();
	}

	for (int i = 0; i < time; i++) {
		j--;
		threads[j].join();
	}
	while (queue_Electronics.end == queue_Electronics.begin){
	    Sleep(100);
	}
	clock_t t1_r, t2_r, t3_r;
	t1_r = queue_body.end - queue_body.begin;
	t2_r = queue_engine.end - queue_engine.begin;
	t3_r = queue_Electronics.end - queue_Electronics.begin;

	std::cout << "\n" << a << " " << " " << b << " " <<
	             c << " "<< t1_r <<
	             " " << t2_r << " " << t3_r;

	std::cout << "\n" << time << " " << t1_r <<
	             " " << t2_r << " " << t3_r;

	for (int i = 0; i < amount; i++) {
		std::cout << "\n" << 1 << " " << queue_body.times_in[i] << " " << queue_body.times_out[i];
		std::cout << "\n" << 2 << " " << queue_engine.times_in[i] << " " << queue_engine.times_out[i];
		std::cout << "\n" << 3 << " " << queue_Electronics.times_in[i] << " " << queue_Electronics.times_out[i];
	}

}

int main()
{
	Car::count = 1;
	CarQueue::count = 1;

	size_t amount = 5;
	CarQueue Cars(amount);

	CarQueue queue_engine(add_engine, 90);
	CarQueue queue_body(add_body, 30);
	CarQueue queue_weapon(add_Electronics, 200);

	CarQueue ready_Cars;

	std::thread give_to_body([&Cars, &queue_body]() { Cars.start(queue_body); });
	std::thread give_to_engine([&queue_body,
		&queue_engine]() {
		queue_body.start(queue_engine);
	});

	std::thread give_to_weapon([&queue_engine,
		&queue_weapon]() {
		queue_engine.start(queue_weapon);
	});
/*
	std::thread give_to_ready([&queue_weapon,
		&ready_Cars]() {
		queue_weapon.start(ready_Cars);
	});
	*/
	give_to_body.join();
	give_to_engine.join();
	give_to_weapon.join();
	//give_to_ready.join();

	std::cout << "\n\ni finished! " << ready_Cars.size() << " " << queue_weapon.size() << queue_engine.size() << queue_body.size();

	return 0;
}

/*
int main()
{
	Car::count = 1;
	CarQueue::count = 1;

	for (int i = 1; i < 11; i++)
		test(5, 1, 700, 1400, 900);

	
	test(5, 1, 100, 100, 2800);
	test(5, 1, 1000, 1000, 1000);
	test(5, 1, 2800, 100, 100);
	for (int i = 100; i < 1000; i += 200) {
	test(5, 1, 1000 - i, 1000 + i, 1000);
	std::cout << " " << i * 2;
	}
	std::cout << "\n";
	for (int i = 100; i < 1000; i += 200) {
	test(5, 1, 1000 + i, 1000, 1000 - i);
	std::cout << " " << i * 2;
	}
	std::cout << "\n";
	for (int i = 100; i < 1000; i += 200) {
	test(5, 1, 1000, 1000 - i, 1000 + i);
	std::cout << " " << i * 2;
	}
	

	test(5, 1, 1000, 500, 1500);

	return 0;
}
*/
