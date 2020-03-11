/**
	\file cImageWindowWidget.h
	\brief Image window widget.
*/


#ifndef _CIMAGEWINDOWWIDGET_H
#define _CIMAGEWINDOWWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QPixmap>
#include <QImage>
#include <unordered_set>
#include "core/utilities.h"


/**
	\brief Layer types.
*/
enum eLayerType {
	layer_compounds = 0,
	layer_optical_image,
	layer_histology_image,
	layer_microscopy_navigation_image,
	layer_microscopy_image_1,
	layer_microscopy_image_2,
	layer_microscopy_image_3,
	layer_microscopy_image_4,
	layer_microscopy_image_5,
	layer_microscopy_image_6,
	layer_microscopy_image_7,
	layer_microscopy_image_8,
	layer_microscopy_image_9,
	layer_microscopy_image_10,
	layer_end
};


/**
	\brief Cursor activity.
*/
enum eCursorActivityType {
	cursoractivity_none = 0,
	cursoractivity_move,
	cursoractivity_resize_left,
	cursoractivity_resize_right,
	cursoractivity_resize_top,
	cursoractivity_resize_bottom,
	cursoractivity_resize_top_left,
	cursoractivity_resize_top_right,
	cursoractivity_resize_bottom_left,
	cursoractivity_resize_bottom_right
};


/**
	\brief Paramaters of a layer.
*/
struct layerInfo {
	/**
		\brief True if the layer is visible.
	*/
	bool checked;
	
	
	/**
		\brief Opacity of the layer.
	*/
	int alpha;


	/**
		\brief Z-value of the layer.
	*/
	int zvalue;


	/**
		\brief True if pixmap is defined.
	*/
	bool ispixmapdefined;


	/**
		\brief True if the pixmap is flipped horizontally.
	*/
	bool fliphorizontally;


	/**
		\brief True if the pixmap is flipped vertically.
	*/
	bool flipvertically;


	/**
		\brief X position.
	*/
	double x;


	/**
		\brief Y position.
	*/
	double y;


	/**
		\brief Width.
	*/
	double width;


	/**
		\brief Height.
	*/
	double height;


	/**
		\brief Rotation angle [degrees].
	*/
	double angle;


	/**
		\brief Last X position.
	*/
	qreal lastx;


	/**
		\brief Last Y position.
	*/
	qreal lasty;


	/**
		\brief Pixmap.
	*/
	QPixmap* pixmap;


	/**
		\brief The constructor.
	*/
	layerInfo() {
		checked = true;
		alpha = 100;
		zvalue = 0;
		ispixmapdefined = false;
		fliphorizontally = false;
		flipvertically = false;
		x = 0;
		y = 0;
		width = 0;
		height = 0;
		angle = 0;
		lastx = 0;
		lasty = 0;
		pixmap = new QPixmap();
	}


	/**
		\brief The copy constructor.
	*/
	layerInfo(const layerInfo& layer) {
		checked = layer.checked;
		alpha = layer.alpha;
		zvalue = layer.zvalue;
		ispixmapdefined = layer.ispixmapdefined;
		fliphorizontally = layer.fliphorizontally;
		flipvertically = layer.flipvertically;
		x = layer.x;
		y = layer.y;
		width = layer.width;
		height = layer.height;
		angle = layer.angle;
		lastx = layer.lastx;
		lasty = layer.lasty;
		pixmap = new QPixmap();
		*pixmap = *layer.pixmap;
	}


	/**
		\brief The destructor.
	*/
	~layerInfo() {
		delete pixmap;
	}
};



/**
	\brief Image window widget.
*/
class cImageWindowWidget : public QGraphicsView
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cImageWindowWidget();


	/**
		\brief The destructor.
	*/ 
	~cImageWindowWidget();


	/**
		\brief Set the optical image.
		\param image an optical image
	*/
	void setOpticalImage(QImage* image);


	/**
		\brief Set the histology image.
		\param histologyimage a histology image
	*/
	void setHistologyImage(QImage* histologyimage);


	/**
		\brief Set the microscopy image.
		\param layer a microscopy layer
		\param microscopyimage a microscopy image
	*/
	void setMicroscopyImage(eLayerType layer, QImage* microscopyimage);


	/**
		\brief Get the image.
		\retval QImage image
	*/ 
	QImage getImage();


	/**
		\brief Set options which was used to filter the points.
		\param coordinateinfo a vector of coordinates
		\param operatortype the type of operator (0 = or; 1 = and)
		\param columnname1 name of column which was compared
		\param comparatorname1 name of used comparator
		\param filterstring1 a string used to filter the points
		\param columnname2 name of column which was compared
		\param comparatorname2 name of used comparator
		\param filterstring2 a string used to filter the points
		\param casesensitive true if the string was used as a casesensitive, false otherwise
		\param wholeword true if whole words only are compared, false otherwise
	*/
	void setFilterOptions(vector<cCoordinateInfo>& coordinateinfo, bool operatortype, string& columnname1, string& comparatorname1, string& filterstring1, string& columnname2, string& comparatorname2, string& filterstring2, bool casesensitive, bool wholeword);


	/**
		\brief Set the values to corelate data with optical image.
		\param leftshift left shift
		\param topshift top shift
		\param maxx maximum X coordinate
		\param maxy maximum Y coordinate
		\param pixelsizex pixel size x
		\param pixelsizey pixel size y
	*/
	void setCorrelationValues(int leftshift, int topshift, int maxx, int maxy, int pixelsizex, int pixelsizey);


	/**
		\brief Set the default maximum X and Y coordinates - parsed from the imzML file.
		\param defaultmaxx maximum X coordinate
		\param defaultmaxy maximum Y coordinate
		\param defaultpixelsizex pixel size x
		\param defaultpixelsizey pixel size y
		\param vendor vendor type
	*/
	void setDefaultMaxXY(int defaultmaxx, int defaultmaxy, int defaultpixelsizex, int defaultpixelsizey, eVendorType vendor);


	/**
		\brief Set the position of a histology image.
		\param x x coordinate
		\param y y coordinate
		\param width width
		\param height height
		\param angle angle
	*/
	void setHistologyPosition(int x, int y, int width, int height, double angle);


	/**
		\brief Set the position of a microscopy image.
		\param layer a microscopy layer
		\param x x offset [um]
		\param y y offset [um]
		\param width width [um]
		\param height height [um]
		\param angle angle
	*/
	void setMicroscopyPosition(eLayerType layer, double x, double y, double width, double height, double angle);


	/**
		\brief Go to the position of a microscopy image.
		\param layer a microscopy layer
	*/
	void goToMicroscopyPosition(eLayerType layer);


	/**
		\brief Set the current region.
		\param xfrom minimum x coordinate
		\param xto maximum x coordinate
		\param yfrom minimum y coordinate
		\param yto maximum y coordinate
	*/
	void setSelectedRegion(int xfrom, int xto, int yfrom, int yto);


	/**
		\brief Redraw widget.
	*/
	void redraw();


protected:


	/**
		\brief Handle the mouse wheel event.
		\param event pointer to QWheelEvent
	*/ 
	void wheelEvent(QWheelEvent *event);


	/**
		\brief Handle the mouse press event.
		\param event pointer to QMouseEvent
	*/ 
	void mousePressEvent(QMouseEvent *event);


	/**
		\brief Handle the mouse move event.
		\param event pointer to QMouseEvent
	*/ 
	void mouseMoveEvent(QMouseEvent *event);


	/**
		\brief Handle the mouse release event.
		\param event pointer to QMouseEvent
	*/
	void mouseReleaseEvent(QMouseEvent *event);


	/**
		\brief Handle the mouse double click event.
		\param event pointer to QMouseEvent
	*/
	void mouseDoubleClickEvent(QMouseEvent *event);


	/**
		\brief Handle the resize event.
		\param event pointer to QResizeEvent
	*/
	void resizeEvent(QResizeEvent *event);


signals:


	/**
		\brief The signal is emitted when the selection of points was changed.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/
	void updateFilter(int xmin, int xmax, int ymin, int ymax);


	/**
		\brief The signal is emitted when the points area in the optical image was changed.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/
	void updateCorrelation(int xmin, int xmax, int ymin, int ymax);


	/**
		\brief The signal is emitted when the histology position was changed.
		\param x x coordinate
		\param y y coordinate
		\param width width
		\param height height
		\param angle angle
	*/
	void updateHistologyPosition(int x, int y, int width, int height, double angle);


	/**
		\brief The signal is emitted when the microscopy position was changed.
		\param x x coordinate [um]
		\param y y coordinate [um]
		\param width width [um]
		\param height height [um]
		\param angle angle
	*/
	void updateMicroscopyPosition(double x, double y, double width, double height, double angle);


	/**
		\brief The signal is emitted when the ruler size was changed.
		\param size ruler size
	*/
	void updateRuler(double size);


	/**
		\brief The image was double clicked.
		\param spectrumid id of a spectrum
	*/
	void imageWidgetDoubleClicked(int spectrumid);


private:

	QWidget* parent;
	QGraphicsScene* scene;

	vector<cCoordinateInfo> coordinateinfo;
	bool operatortype;
	string columnname1;
	string comparatorname1;
	string filterstring1;
	string columnname2;
	string comparatorname2;
	string filterstring2;
	bool casesensitive;
	bool wholeword;

	bool colorscale;
	bool absoluteintensity;

	vector<layerInfo> layersvector;
	int activelayer;

	int xfrom;
	int xto;
	int yfrom;
	int yto;

	int leftshift;
	int topshift;
	int maxx;
	int maxy;
	int pixelsizex;
	int pixelsizey;

	int defaultmaxx;
	int defaultmaxy;
	int defaultpixelsizex;
	int defaultpixelsizey;
	eVendorType vendor;

	qreal currentscale;
	qreal factor;
	
	int rulersize;
	double rulervalue;

	int pressedx;
	int pressedy;
	int currentx;
	int currenty;
	int currentwidth;
	int currentheight;

	bool showselection;

	qreal microscopynavigationcenterx;
	qreal microscopynavigationcentery;

	QGraphicsRectItem* selectionrect;
	QGraphicsSimpleTextItem* selectionsimpletextitem;

	QGraphicsRectItem* currentrect;
	eCursorActivityType cursoractivity;
	
	void redrawScene();

	void drawMicroscopyImage(eLayerType layer, QRectF& rect_scene, int currentwidth, int currentheight);

	void updateSelectionGroup();

	void getRectanglePoints(qreal& rx1, qreal& ry1, qreal& rx2, qreal& ry2);

	double prepareTransformation(QRectF rect, QTransform& transform, bool testedfliphorizontally, bool testedflipvertically, double testedangle, bool flipaxes);
	
	void fixRectCenter(qreal& rx1, qreal& ry1, qreal& rx2, qreal& ry2, bool testedfliphorizontally, bool testedflipvertically, double testedangle);

	double recalculateRulerValue(qreal currentscale);


private slots:


	void zoomIn();


	void zoomOut();


	void normalSize();


	void setRulerValue(double value);


	void scrollbarValueChanged(int value);


	void absoluteIntensityStateChanged(bool state);


	void colorScaleStateChanged(bool state);


	void changeLayer(int layerid, bool checked, int alpha, int zvalue);

	
	void changeActiveLayer(int layerid);


	void flipHistologyHorizontallyStateChanged(bool state); 


	void flipHistologyVerticallyStateChanged(bool state);


	void showSelectionStateChanged(bool state);

};

#endif
