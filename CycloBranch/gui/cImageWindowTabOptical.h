/**
	\file cImageWindowTabOptical.h
	\brief The implementation of a tab used to import images from light microscopy.
*/


#ifndef _CIMAGEWINDOWTABOPTICAL_H
#define _CIMAGEWINDOWTABOPTICAL_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QFileDialog>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "core/utilities.h"


using namespace xercesc;


/**
	\brief Color channels.
*/
enum eChannelType {
	gray = 0,
	red = 1,
	green = 2,
	blue = 3
};


/**
	\brief Description of an image tile.
*/
struct cTileInfo {

	/**
		\brief Tile logical x coordinate.
	*/
	int fieldx;


	/**
		\brief Tile logical y coordinate.
	*/
	int fieldy;


	/**
		\brief Tile position x [m].
	*/
	double posx;


	/**
		\brief Tile position y [m].
	*/
	double posy;


	/**
		\brief The constructor.
	*/
	cTileInfo() {
		fieldx = 0;
		fieldy = 0;
		posx = 0;
		posy = 0;
	}


	/**
		\brief Print the metadata.
	*/
	void print() {
		cout << "fieldx: " << fieldx << endl;
		cout << "fieldy: " << fieldy << endl;
		cout << "posx: " << posx << endl;
		cout << "posy: " << posy << endl;
	}

};


/**
	\brief A structure to keep parsed lif metadata.
*/
struct cLifMetadata {

	/**
		\brief Image name.
	*/
	string imagename;


	/**
		\brief Type of microscopy technique (-1 = unknown, 0 = optical, 1 = fluorescence).
	*/
	int microscopytype;


	/**
		\brief Microscope model.
	*/
	string microscopemodel;


	/**
		\brief Inverse microscope model.
	*/
	int inversemicroscopemodel;


	/**
		\brief Magnification.
	*/
	int magnification;


	/**
		\brief Flip X.
	*/
	int flipx;


	/**
		\brief Flip Y.
	*/
	int flipy;


	/**
		\brief Width [pixels].
	*/
	int dimx;


	/**
		\brief Height [pixels].
	*/
	int dimy;


	/**
		\brief Depth [layers].
	*/
	int dimz;


	/**
		\brief Number of tiles.
	*/
	int dim10;


	/**
		\brief Width [m].
	*/
	double width;


	/**
		\brief Height [m].
	*/
	double height;


	/**
		\brief Stage position x [m].
	*/
	double stageposx;


	/**
		\brief Stage position y [m].
	*/
	double stageposy;


	/**
		\brief Memory block size.
	*/
	long long unsigned memoryblocksize;


	/**
		\brief Memory block ID.
	*/
	string memoryblockid;


	/**
		\brief Offsets of memory blocks.
	*/
	vector<long long unsigned> memoryblockoffsets;


	/**
		\brief Color channels.
	*/
	vector<eChannelType> channels;


	/**
		\brief LUTs.
	*/
	vector<string> luts;


	/**
		\brief Flip X from TileScanInfo.
	*/
	int tilescaninfoflipx;


	/**
		\brief Flip Y from TileScanInfo.
	*/
	int tilescaninfoflipy;


	/**
		\brief Vector of tiles.
	*/
	vector<cTileInfo> tiles;


	/**
		\brief The constructor.
	*/
	cLifMetadata() {
		imagename.clear();
		microscopytype = -1;
		microscopemodel.clear();
		inversemicroscopemodel = 0;
		magnification = 0;
		flipx = 0;
		flipy = 0;
		dimx = 0;
		dimy = 0;
		dimz = 0;
		dim10 = 0;
		width = 0;
		height = 0;
		stageposx = 0;
		stageposy = 0;
		memoryblocksize = 0;
		memoryblockid = "";
		memoryblockoffsets.clear();
		channels.clear();
		luts.clear();
		tilescaninfoflipx = -1;
		tilescaninfoflipy = -1;
		tiles.clear();
	}


	/**
		\brief Print metadata.
	*/
	void print() {
		cout << "imagename: " << imagename << endl;
		cout << "microscopytype: " << microscopytype << endl;
		cout << "microscopemodel: " << microscopemodel << endl;
		cout << "inversemicroscopemodel: " << inversemicroscopemodel << endl;
		cout << "magnification: " << magnification << endl;
		cout << "flipx: " << flipx << endl;
		cout << "flipy: " << flipy << endl;
		cout << "dimx: " << dimx << endl;
		cout << "dimy: " << dimy << endl;
		cout << "dimz: " << dimz << endl;
		cout << "dim10: " << dim10 << endl;
		cout << "width: " << width << endl;
		cout << "height: " << height << endl;
		cout << "stageposx: " << stageposx << endl;
		cout << "stageposy: " << stageposy << endl;
		cout << "memoryblocksize: " << memoryblocksize << endl;
		cout << "memoryblockid: " << memoryblockid << endl;

		for (int i = 0; i < (int)memoryblockoffsets.size(); i++) {
			cout << "memoryblockoffset " << i + 1 << ": " << memoryblockoffsets[i] << endl;
		}

		for (int i = 0; i < (int)channels.size(); i++) {
			cout << "channel " << i + 1 << ": " << channels[i] << endl;
		}

		for (int i = 0; i < (int)luts.size(); i++) {
			cout << "lut " << i + 1 << ": " << luts[i] << endl;
		}

		cout << "tilescaninfoflipx: " << tilescaninfoflipx << endl;
		cout << "tilescaninfoflipy: " << tilescaninfoflipy << endl;

		double avgposx = 0;
		double avgposy = 0;

		for (auto& it : tiles) {
			it.print();

			avgposx += it.posx;
			avgposy += it.posy;
		}

		if (tiles.size() > 0) {
			avgposx /= (double)tiles.size();
			avgposy /= (double)tiles.size();

			cout << "avgposx: " << avgposx << endl;
			cout << "avgpoxy: " << avgposy << endl;
		}

		cout << endl;
	}

};


/**
	\brief Get metadata from a lif file.
	\param filename input lif file
	\param parsedmetadata output vector of parsed metadata
*/
void readLifMetadata(string filename, vector<cLifMetadata>& parsedmetadata);


/**
	\brief Parse metadata from a subtree of a lif file.
	\param subtree root of a subtree
	\param parsedmetadata output vector of parsed metadata
*/
void parseLifMetadataSubTree(DOMElement* subtree, vector<cLifMetadata>& parsedmetadata);


/**
	\brief Parse metadata from a lif file.
	\param metadata input metadata
	\param parsedmetadata output vector of parsed metadata
*/
void parseLifMetadata(string& metadata, vector<cLifMetadata>& parsedmetadata);


/**
	\brief The class representing a tab used to import images from light microscopy.
*/
class cImageWindowTabOptical : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/
	explicit cImageWindowTabOptical(QWidget *parent = 0);


	/**
		\brief Initialize values in the form.
		\param lastdirmicroscopyimage a default directory with microscopy images
		\param numberoflayers number of layers
	*/
	void setFormValues(QString& lastdirmicroscopyimage, int numberoflayers);


	/**
		\brief Get the values from the form.
		\param startfromindex starting index to open microscopy images
		\param tiffilenames a list of tif images to be opened
		\param navigationindex index of navigation image
		\param liffilename a lif file from which the metadata are read
	*/
	void getFormValues(int& startfromindex, QStringList& tiffilenames, int& navigationindex, QString& liffilename);


	/**
		\brief The destructor.
	*/
	~cImageWindowTabOptical();


private:

	QString* lastdirmicroscopyimage;

	QGridLayout* gridlayout;

	QLabel* labelstartfrom;
	QComboBox* comboboxstartfrom;

	QLabel* labelusevaluesfrom;
	QComboBox* comboboxusevaluesfrom;

	QLabel* labellif;
	QLineEdit* lineeditlif;
	QPushButton* buttonlif;

	QLabel* labelfilenames;
	QListWidget* listwidgetfilenames;
	QPushButton* buttonfilenames;
	QStringList imagenames;


private slots:

	void browseButtonReleased();

	void lifButtonReleased();


};


#endif

