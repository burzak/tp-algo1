#include "drone.h"

Drone::Drone()
{
}

Drone::Drone(ID i, const std::vector<Producto>& ps)
{
	_id = i;
	setBateria(100);
	_enVuelo = false;
	_productos = ps;
}

ID Drone::id() const
{
	return _id;
}

Carga Drone::bateria() const
{
	return _bateria;
}

bool Drone::enVuelo() const
{
	return _enVuelo;
}

const Secuencia<Posicion>& Drone::vueloRealizado() const
{
	return _trayectoria;
}

Posicion Drone::posicionActual() const
{
	return _posicionActual;
}

const Secuencia<Producto>& Drone::productosDisponibles() const
{
	return _productos;
}

bool Drone::vueloEscalerado() const
{
	bool res = _enVuelo;

	int i = 0;
	while (i+2 < (int) _trayectoria.size()) {
		int distanciaX = (_trayectoria[2].x - _trayectoria[0].x);
		int distanciaY = (_trayectoria[2].y - _trayectoria[0].y);

		int coord1 = (_trayectoria[i+2].x - _trayectoria[i].x);
		int coord2 = (_trayectoria[i+2].y - _trayectoria[i].y);

		res = res && distanciaX == coord1 && distanciaY == coord2 && coord1 != 0 && coord2 != 0;
		i++;
	}

	return res;
}

Secuencia<InfoVueloCruzado> Drone::vuelosCruzados(const Secuencia<Drone>& ds)
{
	Secuencia<InfoVueloCruzado> vuelosCruzados;
	unsigned int n = 0;
	while (n < ds.size()) {
		unsigned int j = 0;
		while (j < ds[n].vueloRealizado().size()) {
			if (cantidadCruces(ds, ds[n].vueloRealizado()[j], ds[n].vueloRealizado().size()) > 1) {
				InfoVueloCruzado info;
				info.posicion = ds[n].vueloRealizado()[j];
				info.cantidadCruces = cantidadCruces(ds, ds[n].vueloRealizado()[j], ds[n].vueloRealizado().size());
				vuelosCruzados.push_back(info);
			}
			j++;
		}
		n++;
	}
	return Secuencia<InfoVueloCruzado>();
}

void Drone::mostrar(std::ostream & os) const
{
}

void Drone::guardar(std::ostream & os) const
{
}

void Drone::cargar(std::istream & is)
{
}

void Drone::moverA(const Posicion pos)
{
	_enVuelo = true;
	_trayectoria.push_back(pos);
	cambiarPosicionActual(pos); // por invariante agregamos esta linea
	// debemos verificar invariante movimientoOK? pos tiene que ser un movimiento valido?
}

void Drone::setBateria(const Carga c)
{
	_bateria = c;
}

void Drone::borrarVueloRealizado()
{
	_enVuelo = false;
	_trayectoria = std::vector<Posicion>();
}

void Drone::cambiarPosicionActual(const Posicion p)
{
	_posicionActual = p;
}

void Drone::sacarProducto(const Producto p)
{
	int indiceProducto;
	unsigned n=0;
	while (n < _productos.size()) {
		if (_productos[n] == p) {
			indiceProducto = n;
		}
	}
	_productos.erase(_productos.begin() + indiceProducto);
}

bool Drone::operator==(const Drone & otroDrone) const
{
	bool res = true;
	res = res && id() == otroDrone.id();
	res = res && bateria() == otroDrone.bateria();
	res = res && enVuelo() == otroDrone.enVuelo();
	res = res && vueloRealizado() == otroDrone.vueloRealizado();
	res = res && posicionActual() == otroDrone.posicionActual();
	res = res && mismosProductos(productosDisponibles(), otroDrone.productosDisponibles());
	return res;
}

std::ostream & operator<<(std::ostream & os, const Drone & d)
{
	return os;
}

/******************** AUX **************************/
bool Drone::mismosProductos(const Secuencia<Producto> lista1, const Secuencia<Producto> lista2) {
  bool res = lista1.size() == lista2.size();
  unsigned int n = 0;
  while (res && n < lista1.size()) {
		res = res && (cantidad(lista1, lista1[n]) == cantidad(lista2, lista1[n]));
    res = res && (cantidad(lista1, lista2[n]) == cantidad(lista2, lista2[n]));
    n++;
  }

  return res;
}

int Drone::cantidad(const Secuencia<Producto> lista, Producto producto) {
  unsigned int n = 0;
  int cant = 0;
  while (n < lista.size()) {
    if (lista[n] == producto) {
      cant++;
    }
    n++;
  }
  return cant;
}

int Drone::cantidadCruces(const Secuencia<Drone>& ds, Posicion pos, int longitud) {
	int n = 0;
	int total = 0;
	while (n < longitud) {
		int cant = 0;
		unsigned int j = 0;
		while (j < ds.size()) {
			if (ds[j].vueloRealizado()[n] == pos) {
				cant++;
			}
		}
		if (cant > 1) {
			total = total + cant;
		}
		cant = 0;
		n++;
	}
	return total;
}
