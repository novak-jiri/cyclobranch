/**
	\file cPubChemSearchWidget.h
	\brief The implementation of the dialog PubChem search widget.
*/


#ifndef _CPUBCHEMSEARCHWIDGET_H
#define _CPUBCHEMSEARCHWIDGET_H

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtConcurrent>
#include <QApplication>
#include <QThreadPool>

#include "core/utilities.h"


// forward declaration
class QTextBrowser;
class QVBoxLayout;


/**
	\brief The widget representing the PubChem search widget.
*/
class cPubChemSearchWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cPubChemSearchWidget();


	/**
		\brief The destructor.
	*/ 
	~cPubChemSearchWidget();


	/**
		\brief Set the HTML text into the widget.
		\param html string with the HTML text
	*/ 
	void setHTML(string html);


	/**
		\brief Get the HTML text from the widget.
		\retval string HTML string
	*/ 
	string getHTML();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Store the content into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the content from an input stream.
		\param is an input stream
	*/ 
	void load(ifstream& is);


private:

	QVBoxLayout* layout;
	QTextBrowser* textbrowser;
	string htmlstring;


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


};


/**
	\brief The functor used to get the CIDs of compounds with the same formula.
*/
struct cPubchemCIDReader {

	/**
		\brief result_type
	*/
	typedef string result_type;


	/**
		\brief Get the list of CIDs.
		\param compound formula
		\retval string list of CIDs
	*/
	string getCIDList(const string& compound) {
		string url = "https://pubchem.ncbi.nlm.nih.gov/rest/pug/compound/fastformula/";
		url += compound;
		url += "/cids/txt";

		QNetworkAccessManager networkmanager;
		QNetworkRequest request(QUrl(url.c_str()));
		QNetworkReply *response = networkmanager.get(request);

		QEventLoop loop;
		QObject::connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
		loop.exec();

		string cids = response->readAll().toStdString();

		stringstream errorstream;
		if (cids.empty()) {
			errorstream << "@ERROR" << endl;
			errorstream << "URL: " << url << endl;
			errorstream << "ERROR: " << response->errorString().toStdString() << endl;
			errorstream << endl;
		}

		response->deleteLater();

		if (cids.empty()) {
			return errorstream.str();
		}

		return cids;
	}


	/**
		\brief Get a list of CIDs separated by comma.
		\param cids input list of CIDs
		\param reducedcids output list of CIDs separated by comma
	*/
	void getCommaSeparatedCIDs(string& cids, string& reducedcids) {
		reducedcids.clear();
		if (cids.find("Status:") == string::npos) {
			istringstream inputstream(cids);
			string line;
			while (getline(inputstream, line)) {
				if (!reducedcids.empty()) {
					reducedcids += ",";
				}
				reducedcids += line;
			}
		}
	}


	/**
		\brief Attach a vector of CIDs.
		\param cids input list of CIDs
		\param cidsvector input/output vector of CIDs
	*/
	void attachVectorOfCIDs(string& cids, vector<string>& cidsvector) {
		if (cids.find("Status:") == string::npos) {
			istringstream inputstream(cids);
			string line;
			while (getline(inputstream, line)) {
				cidsvector.push_back(line);
			}
		}
	}


	/**
		\brief Get the number of compounds with the same formula.
		\param cids list of CIDs
		\retval int number of CIDs
	*/
	int getNumberOfCIDs(string& cids) {
		int count = 0;
		if (cids.find("Status:") == string::npos) {
			istringstream inputstream(cids);
			string line;
			while (getline(inputstream, line)) {
				count++;
			}
		}
		else {
			if (cids.find("Status: 404") == string::npos) {
				// report error codes 500 (server error), 503 (server busy), etc.
				cout << cids << endl << endl;
			}
		}

		return count;
	}


	/**
		\brief Overloaded operator ().
		\param compound formula
		\retval string list of CIDs
	*/
	string operator()(const string& compound) {
		return getCIDList(compound);
	}
};


/**
	\brief The functor used to get the names of compounds from CID.
*/
struct cPubchemNameReader {

	/**
		\brief result_type
	*/
	typedef string result_type;


	/**
		\brief Get the list of names.
		\param cid CID
		\retval string list of names
	*/
	string getNameList(const string& cid) {
		bool usepost = true;

		string url = "https://pubchem.ncbi.nlm.nih.gov/rest/pug/compound/cid/";
		if (!usepost) {
			url += cid + "/";
		}
		url += "synonyms/txt";

		if (!usepost) {
			// URL too long
			if (url.size() > 4000) {
				return "Status: 414";
			}
		}

		QNetworkAccessManager networkmanager;
		QNetworkRequest request(QUrl(url.c_str()));
		QNetworkReply *response;
		
		if (usepost) {
			QString body = "cid=";
			body += cid.c_str();
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
			response = networkmanager.post(request, body.toUtf8());
		}
		else {
			response = networkmanager.get(request);
		}

		QEventLoop loop;
		QObject::connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
		loop.exec();

		string names = response->readAll().toStdString();

		stringstream errorstream;
		if (names.empty()) {
			errorstream << "@ERROR" << endl;
			errorstream << "URL: " << url << endl;
			errorstream << "ERROR: " << response->errorString().toStdString() << endl;
			errorstream << endl;
		}

		response->deleteLater();

		if (names.empty()) {
			return errorstream.str();
		}

		return names;
	}


	/**
		\brief Get the reduded list of names.
		\param names input list of names
		\param reducednames ouput set of reduced names
	*/
	void getReducedNames(string& names, set<string>& reducednames) {
		if (names.find("Status:") == string::npos) {
			istringstream inputstream(names);
			string line;
			while (getline(inputstream, line)) {
				if (line.find("SCHEMBL") == string::npos) {
					reducednames.insert(line);
				}
			}
			if (reducednames.empty()) {
				reducednames.insert("This item has only SCHEMBL name(s).");
			}
		}
		else {
			if (names.find("Status: 404") == string::npos) {
				// report error codes 500 (server error), 503 (server busy), etc.
				cout << names << endl << endl;
			}
		}
	}


	/**
		\brief Overloaded operator ().
		\param cid CID
		\retval string list of names
	*/
	string operator()(const string& cid) {
		if (cid.empty()) {
			return "";
		}
		return getNameList(cid);
	}
};


/**
	\brief An auxiliary row info structure.
*/
struct cRowItemInfo {

	/**
		\brief Identification ID.
	*/
	int id;


	/**
		\brief Theoretical m/z.
	*/
	double theoreticalmz;


	/**
		\brief Experimental intensity.
	*/
	double experimentalint;


	/**
		\brief Ion type.
	*/
	string iontype;

};

#endif

