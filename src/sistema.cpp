#include "sistema.h"
#include <algorithm>

Sistema::Sistema()
{
}

Sistema::Sistema(const Campo & c, const Secuencia<Drone>& ds)
{
	_campo = c;
	Dimension dim = _campo.dimensiones();

	// revisar/preguntar porque no podemos pasarle las dimensiones a Grilla
	_estado = Grilla<EstadoCultivo>();

	int i = 0;
	while (i < dim.ancho) {
		int j = 0;
		while (j < dim.largo) {
			_estado.parcelas[i][j] = NoSensado;
		}
	}

	_enjambre = ds;
}

const Campo & Sistema::campo() const
{
	return _campo;
}

EstadoCultivo Sistema::estadoDelCultivo(const Posicion & p) const
{
	return EstadoCultivo();
}

const Secuencia<Drone>& Sistema::enjambreDrones() const
{
	return _enjambre;
}

void Sistema::crecer()
{
	int i = 0;
	while(i < _campo.dimensiones().ancho){
			int j = 0;
			while (j < _campo.dimensiones().largo) {
				if (_estado.parcelas[i][j] == RecienSembrado){
					_estado.parcelas[i][j] = EnCrecimiento;
				}
				if (_estado.parcelas[i][j] == EnCrecimiento){
					_estado.parcelas[i][j] = ListoParaCosechar;
				}
				j++;
			}
		i++;
	}
}

void Sistema::seVinoLaMaleza(const Secuencia<Posicion>& ps)
{
	unsigned int i = 0;
	while(i < ps.size()){
		_estado.parcelas[ps[i].x][ps[i].y] = ConMaleza;
		i++;
	}
}

void Sistema::seExpandePlaga()
{
	int i = 0;
	while (i < _campo.dimensiones().ancho){
		int j = 0;
		while (j < _campo.dimensiones().largo){
			//Es necesario verificar que las parcelas vecinas existan.
			if(enRangoConPlaga(i+1,j)||enRangoConPlaga(i-1,j)||enRangoConPlaga(i,j+1)||enRangoConPlaga(i,j-1)){
				_estado.parcelas[i][j] = ConMaleza;
			}
			j++;
		}
		i++;
	}
}

void Sistema::despegar(const Drone & d)
{
	Posicion pos;

	if (parcelaLibre(d.posicionActual().x + 1, d.posicionActual().y)){
		pos.x = d.posicionActual().x + 1;
		pos.y = d.posicionActual().y;
	}
	if (parcelaLibre(d.posicionActual().x - 1, d.posicionActual().y)){
		pos.x = d.posicionActual().x - 1;
		pos.y = d.posicionActual().y;
	}
	if (parcelaLibre(d.posicionActual().x, d.posicionActual().y + 1)){
		pos.x = d.posicionActual().x;
		pos.y = d.posicionActual().y + 1;
	}
	if (parcelaLibre(d.posicionActual().x, d.posicionActual().y - 1)){
		pos.x = d.posicionActual().x;
		pos.y = d.posicionActual().y - 1;
	}

	unsigned int i = 0;
	while (i < _enjambre.size()){
		if (_enjambre[i].id() == d.id()){
			_enjambre[i].moverA(pos);
		}
		i++;
	}
}

bool Sistema::listoParaCosechar() const
{
	int i = 0;
	float parcelasListas = 0;
	float cantidadParcelas = 0;

	while (i < campo().dimensiones().ancho){
		int j = 0;
		while (j < campo().dimensiones().largo){
			if (_estado.parcelas[i][j] == ListoParaCosechar){
				parcelasListas++;
			}
			cantidadParcelas++;
			j++;
		}
		i++;
	}
	return (parcelasListas/parcelasListas) >= 0.9;
}

void Sistema::aterrizarYCargarBaterias(Carga b)
{
}

void Sistema::fertilizarPorFilas()
{
}

void Sistema::volarYSensar(const Drone & d)
{
	int i = 0;
	//Hay que buscar un dron equivalente al que nos dieron en el enjambre del sistema.
	//Por invariante y requiere deberia siempre encontrarlo y ser unico.
	while (i < enjambreDrones().size()){
		if (enjambreDrones()[i].id() == d.id()){
			//Como quiero modificarlo tengo que usar _enjambre, no enjambreDeDrones() no?
			//Asigno por referencia para poder modificar el drone en cuestion
			Drone& droneUsado = _enjambre[i];
		}
		i++;
	}

	//El granero cuenta como parcelaDisponible? Falta la aux en la especificacion

	Posicion targetPos;
	//Estos muchos ifs no me gustan demasiado
	if (enRangoCultivableLibre(droneUsado.posicionActual().x + 1, droneUsado.posicionActual().y)){
		targetPos.x = droneUsado.posicionActual().x + 1;
		targetPos.y = droneUsado.posicionActual().y;
		droneUsado.moverA(targetPos);
		droneUsado.setBateria(droneUsado.bateria() - 1);
	}
	else if (enRangoCultivableLibre(droneUsado.posicionActual().x - 1, droneUsado.posicionActual().y)){
		targetPos.x = droneUsado.posicionActual().x - 1;
		targetPos.y = droneUsado.posicionActual().y;
		droneUsado.moverA(targetPos);
		droneUsado.setBateria(droneUsado.bateria() - 1);
	}
	else if (enRangoCultivableLibre(droneUsado.posicionActual().x, droneUsado.posicionActual().y + 1)){
		targetPos.x = droneUsado.posicionActual().x;
		targetPos.y = droneUsado.posicionActual().y + 1;
		droneUsado.moverA(targetPos);
		droneUsado.setBateria(droneUsado.bateria() - 1);
	}
	else if (enRangoCultivableLibre(droneUsado.posicionActual().x, droneUsado.posicionActual().y - 1)){
		targetPos.x = droneUsado.posicionActual().x;
		targetPos.y = droneUsado.posicionActual().y - 1;
		droneUsado.moverA(targetPos);
		droneUsado.setBateria(droneUsado.bateria() - 1);
	}

	//Si la parcela esta noSensada se le puede poner cualquier verdura (eh, entienden? Cualquier verdura para cultivar!)
	if (estadoDelCultivo(targetPos) == NoSensado){
		_estado.parcelas[targetPos.x][targetPos.y] = RecienSembrado;
	}
	else {
		
	}


	/***RECORDAR***/
	//Cambiar bateria cuando se mueve (drone.moverA() no cambia la bateria)
	//Cambiar la bateria cuando se aplica un producto o similar




}

void Sistema::mostrar(std::ostream & os) const
{
}

void Sistema::guardar(std::ostream & os) const
{
}

void Sistema::cargar(std::istream & is)
{
}

bool Sistema::operator==(const Sistema & otroSistema) const
{
	return false;
}

std::ostream & operator<<(std::ostream & os, const Sistema & s)
{
	s.mostrar(os);
	return os;
}



/********************** AUX *****************************/
bool Sistema::enRangoConPlaga(int x, int y) const{
	bool res = true;
	res = res && (x >= 0) && (x < _campo.dimensiones().ancho);
	res = res && (y >= 0) && (y < _campo.dimensiones().largo);
	res = res && _estado.parcelas[x][y] == ConPlaga;
	return res;
}

bool Sistema::parcelaLibre(int x, int y) const{
	bool res = true;

	unsigned int i = 0;
	while (i < enjambreDrones().size()){
		bool xDistinto = enjambreDrones()[i].posicionActual().x != x;
		bool yDistinto = enjambreDrones()[i].posicionActual().y != y;
		res = res && (xDistinto && yDistinto);
		i++;
	}

	return res;
}
