#include "GetSet/GetSetSpecial.hxx"
#include "GetSet/GetSetXML.h"
#include "GetSet/Factory.h"
#include "GetSet/Configurator.hxx"

#include "GetSetGui/GetSetGui.h"

#include <iostream>

/// Debugging a tree:
class DebugTree : public GetSetInternal::Access {
public:
	DebugTree(GetSetDictionary& dictionary=GetSetDictionary::global()) : Access(dictionary) {}
	void print() {
		std::string tree;
		assemblePlot(getRoot()->getSection(),tree);
		std::cout << tree;
	}
private:
	typedef std::map<std::string,GetSetInternal::GetSetNode*> PropertyByName;
	void assemblePlot(const PropertyByName& here, std::string& out, std::string indent="")
	{
		for (PropertyByName::const_iterator it=here.begin();it!=here.end();++it)
		{
			GetSetInternal::GetSetSection* s=dynamic_cast<GetSetInternal::GetSetSection*>(it->second); 
			if (s)
			{
				out+=indent+" +-"+it->first+"\n";
				assemblePlot(s->getSection(),out,indent+" | ");
			}
			else
				out+=indent+" +-"+it->first+"\n";
		}
	}
};

///
class Vehicle : public Factory::Object {
protected:
	double		m_speed;
	std::string	m_make;

public:
	Vehicle()
		: m_speed(0)
		, m_make("Unknown Make")
	{}

	virtual void configure(Factory::Configurator& config)
	{
		config.declare("Speed",m_speed);
		config.declare("Make",m_make);
	}


};

class Plane : public  Vehicle {
protected:
	std::string m_pilot;

public:
	Plane()
		: Vehicle()
		, m_pilot("Unknown Pilot")
	{}

	virtual void configure(Factory::Configurator& config)
	{
		Vehicle::configure(config);
		config.declare("Pilot",m_pilot);
	}
};
DECLARE_TYPE(Plane,Vehicle)

///
class Car : public Vehicle {
protected:
	int m_wheels;
	int m_passengers;

public:

	Car()
		: Vehicle()
		, m_wheels(4)
		, m_passengers(5)
	{}

	virtual void configure(Factory::Configurator& config)
	{
		Vehicle::configure(config);
		config.declare("Number Of Wheels",m_wheels);
		config.declare("Max. Pasengers",m_passengers);
	}

};
DECLARE_TYPE(Car,Vehicle)

///
class PickUp : public Car {
protected:
	double m_cargo;

public:
	PickUp()
		: Car()
		, m_cargo(100)
	{
		m_passengers=2;
	}

	virtual void configure(Factory::Configurator& config)
	{
		Car::configure(config);
		config.declare("Max. Cargo",m_cargo);
	}

};
DECLARE_TYPE(PickUp,Car;Vehicle)

///
class House : public Factory::Object {
protected:
	std::string m_street;
	std::string m_city;
	int m_zipCode;

public:
	House()
	{
		m_street="No Street";
		m_city="Unknown City";
		m_zipCode=19999;
	}

	virtual void configure(Factory::Configurator& config)
	{
		config.declare("Street",m_street);
		config.declare("City",m_city);
		config.declare("Zip",m_zipCode);
	}

};
DECLARE_TYPE(House,House)

class RichPerson {
protected:
	std::string m_name;
	Car			m_car;
	Vehicle		*m_additionalVehicle0;
	Vehicle		*m_additionalVehicle1;
	House		m_residence;

public:
	virtual void configure(Factory::Configurator& config)
	{
		config.declare("Name",m_name);
		// We own just one house so we can just use the SAME section to configure it.
		m_residence.configure(config);
		// But the car we want in a subsection
		m_car.configure(config.subsection("Primary Car"));

		// Then, we might own different types of car and additional vehicles. By default, a Pickup and a Plane
		config.create("Additional Vehicle 0",m_additionalVehicle0,"Pickup");
		config.create("Additional Vehicle 1",m_additionalVehicle1,"Plane");
	}

};

void gui(const std::string& section, const std::string& key)
{
	std::cout << section << " - " << key << std::endl;
}

void print(const std::set<std::string>& obj, const std::string& what)
{
	std::cout << what << ":\n" << std::endl;
	for (std::set<std::string>::iterator it=obj.begin();it!=obj.end();++it)
		std::cout << "\t" << *it << std::endl;
}

int main(int argc, char **argv)
{

	Factory::Configurator config;

	print(Factory::KnownTypes(),"All Objects");
	print(Factory::KnownTypes("Vehicle"),"Vehicles");
	print(Factory::KnownTypes("Car"),"Cars");
	print(Factory::KnownTypes("PickUp"),"PickUps");

	Car c;
	c.configure(config.subsection("Some Car"));

	RichPerson p;
	p.configure(config.subsection("Rich Guy"));



	GetSetIO::save(GetSetIO::XmlFile("out.xml"));
	
	DebugTree().print();

	GetSetHandler callback(gui);

	return GetSetGui::runQtApp("Test",argc,argv);
}
