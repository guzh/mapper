/*
 *    Copyright 2012 Thomas Schöps, Kai Pastor
 *    
 *    This file is part of OpenOrienteering.
 * 
 *    OpenOrienteering is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    OpenOrienteering is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _OPENORIENTEERING_PRINT_WIDGET_H_
#define _OPENORIENTEERING_PRINT_WIDGET_H_

#include <QWidget>
#include <QPrinterInfo>

#include "../map_editor.h"
#include "../tool.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QFormLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
QT_END_NAMESPACE

class Map;
class MapPrinter;
class MapPrinterOptions;
class MapPrinterPageFormat;
class MapView;
class MainWindow;
class PrintTool;

/**
 * The print widget lets the user adjust targets and parameters
 * for printing and export.
 */
class PrintWidget : public QWidget
{
Q_OBJECT
public:
	/** Constructs a new print widget. */
	PrintWidget(Map* map, MainWindow* main_window, MapView* main_view, MapEditorController* editor, QWidget* parent = NULL);
	
	virtual ~PrintWidget();
	
	virtual QSize sizeHint() const;
	
public slots:
	/** 
	 * Sets the active state of the print widget.
	 * 
	 * When the widget becomes active, it activates a tool on the map editor
	 * which allows to move the print area. When the widget becomes inactive,
	 * the tool is removed.
	 */
	void setActive(bool state);
	
	/** Sets the widget's (print/export) target. */
	void setTarget(const QPrinterInfo* target);
	
	/** Sets the format of a single page. */
	void setPageFormat(const MapPrinterPageFormat& format);
	
	/** Sets the exported area. */
	void setPrintArea(const QRectF& area);
	
	/** Sets output options: resolution, overprinting. */
	void setOptions(const MapPrinterOptions& parameters);
	
signals:
	/**
	 * This signal is emitted when a print or export job has been started 
	 * and finished. 
	 * 
	 * The signal is not emitted when the widget is hidden 
	 * (cf. QDialog::finished(int result) ).
	 */
	void finished(int result);
	
protected slots:
	/** This slot reacts to changes of the target combobox. */
	void targetChanged(int index) const;
	
	/** This slot reacts to changes of the paper size combobox. */
	void paperSizeChanged(int index) const;
	
	/** This slot reacts to changes of the page orientation combobox. */
	void pageOrientationChanged(int index) const;
	
	/** This slot applies the map area policy to the current area. */
	void applyPrintAreaPolicy() const;
	
	/** This slot reacts to changes of print area position widgets. */
	void printAreaMoved();
	
	/** This slot reacts to changes of print area size widget. */
	void printAreaResized();
	
	/** This slot is called when the resolution widget signals that editing finished. */
	void resolutionEdited();
	
	/** This slot enables the alternative scale widget, or resets it. */
	void differentScaleClicked(bool checked);
	
	/** This slot reacts to changes in the alternative scale widget. */
	void differentScaleEdited(int value);
	
	/** This slot reacts to changes of the "Show template" option. */
	void showTemplatesClicked(bool checked);
	
	/** This slot reacts to changes of the "Show grid" option. */
	void showGridClicked(bool checked);
	
	/** This slot reacts to changes of the "Simulate overprinting" option. */
	void overprintingClicked(bool checked);
	
	void previewClicked();
	void printClicked();
	
protected:
	/** Alternative policies of handling the print area. */
	enum PrintAreaPolicy
	{
		SinglePage         = 2,
		CustomArea         = 4
	};
	
	/** Re-initializes the list of print/export targets. */
	void updateTargets();
	
	/** Updates the list of paper sizes from the given target. */
	void updatePaperSizes(const QPrinterInfo* target) const;
	
	/** Updates the list of resolutions from the given target. */
	void updateResolutions(const QPrinterInfo* target) const;
	
	/** A list of paper sizes which is used when the target does not specify
	 *  supported paper sizes. */
	QList<QPrinter::PaperSize> defaultPaperSizes() const;
	
	/** Moves the given rectangle to a position where it is centered on the
	 *  map for the current output options. */
	void centerOnMap(QRectF& area) const;
	
	/** Shows a warning and returns true if the output would be empty. */
	bool checkForEmptyMap();
	
private:
	enum Exporters
	{
		PdfExporter = -1,
		ImageExporter = -2
	};
	
	QFormLayout* layout;
	
	QComboBox* target_combo;
	QComboBox* paper_size_combo;
	QComboBox* page_orientation_combo;
	QSpinBox* copies_edit;
	
	QComboBox* dpi_combo;
	QCheckBox* show_templates_check;
	QCheckBox* show_grid_check;
	QCheckBox* overprinting_check;
	QCheckBox* different_scale_check;
	QSpinBox* different_scale_edit;
	
	QComboBox* policy_combo;
	QCheckBox* center_check;
	QDoubleSpinBox* left_edit;
	QDoubleSpinBox* top_edit;
	QDoubleSpinBox* width_edit;
	QDoubleSpinBox* height_edit;
	
	QPushButton* preview_button;
	QPushButton* print_button;
	
	QList<QPrinterInfo> printers;
	
	Map* map;
	MapPrinter* map_printer;
	MainWindow* main_window;
	MapView* main_view;
	MapEditorController* editor;
	PrintTool* print_tool;
};

#endif