#include <QDate>
#include <QDebug>
#include "ClipParamDialog.h"

ClipParamDialog::ClipParamDialog( ClipModel& clip, const CategoryData* categoryData, QWidget *parent ) : QDialog(parent), ui(new Ui::ClipParamDialog)
{
    ui->setupUi(this);

    editingClip     = &clip;
    m_pCategoryData = categoryData;
    okOnLeft        = false;
    m_bInitializing = true;

#ifdef Q_WS_WIN
    // Swap buttons for Windows, as is customary
    okOnLeft = true;
    ui->buttonLeft->setText(  tr("OK") );
    ui->buttonRight->setText( tr("Cancel") );
#endif

    // Enable UI bits
    ui->catCheckBox->setChecked(            editingClip->enableCategory );
    ui->catComboBox->setEnabled(            editingClip->enableCategory );

    ui->subcatCheckBox->setEnabled(         editingClip->enableCategory );
    ui->subcatCheckBox->setChecked(         editingClip->enableSubCategory );
    ui->subcatComboBox->setEnabled(         editingClip->enableSubCategory );

    ui->colorCheckBox->setChecked(          editingClip->enableColor );
    ui->colorComboBox->setEnabled(          editingClip->enableColor );

    ui->sizeCheckBox->setChecked(           editingClip->enableSize );
    ui->sizeHorizontalSlider->setEnabled(   editingClip->enableSize );

    ui->ageCheckBox->setChecked(            editingClip->enableAge );
    ui->ageComboBox->setEnabled(            editingClip->enableAge );

    ui->brokenCheckBox->setChecked(         editingClip->enableBroken );
    ui->brokenRadioButtonYes->setEnabled(   editingClip->enableBroken );
    ui->brokenRadioButtonNo->setEnabled(    editingClip->enableBroken );

    ui->missingCheckBox->setChecked(        editingClip->enableMissingParts );
    ui->missingRadioButtonYes->setEnabled(  editingClip->enableMissingParts );
    ui->missingRadioButtonNo->setEnabled(   editingClip->enableMissingParts );

    ui->batCheckBox->setChecked(            editingClip->enableBatteries );
    ui->batRadioButtonYes->setEnabled(      editingClip->enableBatteries );
    ui->batRadioButtonNo->setEnabled(       editingClip->enableBatteries );

    // Set up button groups
    m_pBrokenGroup = new QButtonGroup();
    m_pBrokenGroup->addButton( ui->brokenRadioButtonYes );
    m_pBrokenGroup->addButton( ui->brokenRadioButtonNo );
    m_pBrokenGroup->setExclusive(true);
    connect( m_pBrokenGroup,    SIGNAL( buttonClicked(QAbstractButton*) ), SLOT( onBrokenGroup_clicked(QAbstractButton*) ) );

    m_pMissingGroup = new QButtonGroup();
    m_pMissingGroup->addButton( ui->missingRadioButtonYes );
    m_pMissingGroup->addButton( ui->missingRadioButtonNo );
    m_pMissingGroup->setExclusive(true);
    connect( m_pMissingGroup,   SIGNAL( buttonClicked(QAbstractButton*) ), SLOT( onMissingGroup_clicked(QAbstractButton*) ) );

    m_pBatteriesGroup = new QButtonGroup();
    m_pBatteriesGroup->addButton( ui->batRadioButtonYes );
    m_pBatteriesGroup->addButton( ui->batRadioButtonNo );
    m_pBatteriesGroup->setExclusive(true);
    connect( m_pBatteriesGroup, SIGNAL( buttonClicked(QAbstractButton*) ), SLOT( onBatteriesGroup_clicked(QAbstractButton*) ) );

    if ( m_pCategoryData )
    {
        int index = 0;

        // Add category data
        for ( int i = 0; i < m_pCategoryData->categoryList->size(); i++ )
            ui->catComboBox->addItem( m_pCategoryData->categoryList->at(i)->name );

        // Find category
        if ( editingClip->enableCategory )
            index = m_pCategoryData->getCategoryByName( editingClip->category )->index;

        // Assign category
        ui->catComboBox->setCurrentIndex(index);

        // Populate with subcategories
        for ( int i = 0; i < m_pCategoryData->categoryList->at(index)->subcategories->size(); i++ )
            ui->subcatComboBox->addItem( m_pCategoryData->categoryList->at(index)->subcategories->at(i).toString() );

        // Assign subcategory
        if ( editingClip->enableSubCategory )
        {
            int subIndex = m_pCategoryData->categoryList->at(index)->subcategories->indexOf( editingClip->subCategory );
            ui->subcatComboBox->setCurrentIndex(subIndex);
        }
    }

    if ( editingClip->enableColor )
    {
        ui->colorComboBox->setCurrentText( editingClip->color );
    }

    if ( editingClip->enableSize )
    {
        ui->sizeHorizontalSlider->setValue( editingClip->size );
    }

    // Add years to combo box
    const int startingYear = 1960;
    for ( int i = startingYear; i <= QDate::currentDate().year(); i++ )
        ui->ageComboBox->addItem(QString::number(i));

    if ( editingClip->enableAge )
    {
        ui->ageComboBox->setCurrentText(editingClip->year);
    }

    if ( editingClip->enableBroken )
    {
        ui->brokenRadioButtonYes->setChecked( editingClip->broken );
        ui->brokenRadioButtonNo->setChecked( !editingClip->broken );
    }

    if ( editingClip->enableMissingParts )
    {
        ui->missingRadioButtonYes->setChecked( editingClip->missingParts );
        ui->missingRadioButtonNo->setChecked( !editingClip->missingParts );
    }

    if ( editingClip->enableBatteries )
    {
        ui->batRadioButtonYes->setChecked( editingClip->batteries );
        ui->batRadioButtonNo->setChecked( !editingClip->batteries );
    }

    m_bInitializing = false;
}


ClipParamDialog::~ClipParamDialog()
{
    delete ui;
}


void ClipParamDialog::on_catCheckBox_stateChanged( int value )
{
    qDebug() << "Category toggle state:" << value;

    editingClip->enableCategory = value > 0;

    if ( !m_bInitializing )
        editingClip->category = ui->catComboBox->currentText();

    ui->subcatCheckBox->setEnabled( editingClip->enableCategory );
    ui->catComboBox->setEnabled( editingClip->enableCategory );
}

void ClipParamDialog::on_subcatCheckBox_stateChanged( int value )
{
    qDebug() << "Subcategory toggle state:" << value;

    editingClip->enableSubCategory = value > 0;

    if ( !m_bInitializing )
        editingClip->subCategory = ui->subcatComboBox->currentText();

    ui->subcatComboBox->setEnabled( editingClip->enableSubCategory );
}

void ClipParamDialog::on_colorCheckBox_stateChanged( int value )
{
    qDebug() << "Color toggle state:" << value;

    editingClip->enableColor = value > 0;

    if ( !m_bInitializing )
        editingClip->color = ui->colorComboBox->currentText();

    ui->colorComboBox->setEnabled( editingClip->enableColor );
}

void ClipParamDialog::on_sizeCheckBox_stateChanged( int value )
{
    qDebug() << "Size toggle state:" << value;

    editingClip->enableSize = value > 0;

    if ( !m_bInitializing )
        editingClip->size = value;

    ui->sizeHorizontalSlider->setEnabled( editingClip->enableSize );
}

void ClipParamDialog::on_ageCheckBox_stateChanged( int value )
{
    qDebug() << "Age toggle state:" << value;

    editingClip->enableAge = value > 0;

    if ( !m_bInitializing )
        editingClip->year = ui->ageComboBox->currentText();

    ui->ageComboBox->setEnabled( editingClip->enableAge );
}

void ClipParamDialog::on_brokenCheckBox_stateChanged( int value )
{
    qDebug() << "Broken toggle state:" << value;

    editingClip->enableBroken = value > 0;

    if ( !m_bInitializing )
        editingClip->broken = ui->brokenRadioButtonYes->isChecked();

    ui->brokenRadioButtonYes->setEnabled( editingClip->enableBroken );
    ui->brokenRadioButtonNo->setEnabled(  editingClip->enableBroken );
}

void ClipParamDialog::on_missingCheckBox_stateChanged( int value )
{
    qDebug() << "Missing toggle state:" << value;

    editingClip->enableMissingParts = value > 0;

    if ( !m_bInitializing )
        editingClip->missingParts = ui->missingRadioButtonYes->isChecked();

    ui->missingRadioButtonYes->setEnabled( editingClip->enableMissingParts );
    ui->missingRadioButtonNo->setEnabled(  editingClip->enableMissingParts );
}

void ClipParamDialog::on_batCheckBox_stateChanged( int value )
{
    qDebug() << "Battery toggle state:" << value;

    editingClip->enableBatteries = value > 0;

    if ( !m_bInitializing )
        editingClip->batteries = ui->batRadioButtonYes->isChecked();

    ui->batRadioButtonYes->setEnabled( editingClip->enableBatteries );
    ui->batRadioButtonNo->setEnabled(  editingClip->enableBatteries );
}

void ClipParamDialog::on_buttonRight_clicked()
{
    if ( okOnLeft )
        reject();
    else
        accept();
}

void ClipParamDialog::on_buttonLeft_clicked()
{
    if ( !okOnLeft )
        reject();
    else
        accept();
}


//------------------------------------------------------------------------------------------------------
// Non-Checkbox-Related UI Activites
//------------------------------------------------------------------------------------------------------

void ClipParamDialog::on_catComboBox_currentIndexChanged( int index )
{
    if ( m_bInitializing )
        return;

    QString result = ui->catComboBox->itemText(index);
    qDebug() << "Category changed:" << result;

    editingClip->category = result;
    qDebug() << editingClip->category;

    // Populate with subcategories
    ui->subcatComboBox->clear();
    for ( int i = 0; i < m_pCategoryData->categoryList->at(index)->subcategories->size(); i++ )
    {
        QString subcat = m_pCategoryData->categoryList->at(index)->subcategories->at(i).toString();
        ui->subcatComboBox->addItem( subcat );
    }
}

void ClipParamDialog::on_subcatComboBox_currentIndexChanged( int index )
{
    if ( m_bInitializing )
        return;

    QString result = ui->subcatComboBox->itemText(index);
    qDebug() << "Subcategory changed:" << result;

    editingClip->subCategory = result;
}

void ClipParamDialog::on_colorComboBox_currentIndexChanged( int index )
{
    if ( m_bInitializing )
        return;

    QString result = ui->colorComboBox->itemText(index);
    qDebug() << "Color changed:" << result;

    editingClip->color = result;
}

void ClipParamDialog::on_sizeHorizontalSlider_valueChanged( int value )
{
    if ( m_bInitializing )
        return;

    qDebug() << "Size changed:" << value;

    editingClip->size = value;
}

void ClipParamDialog::on_ageComboBox_currentIndexChanged( int index )
{
    if ( m_bInitializing )
        return;

    QString result = ui->ageComboBox->itemText(index);
    qDebug() << "Year changed:" << result;

    editingClip->year = result;
}

void ClipParamDialog::onBrokenGroup_clicked(QAbstractButton *button)
{
    if ( m_bInitializing )
        return;

    bool value = button == ui->brokenRadioButtonYes;
    if ( value != editingClip->broken )
    {
        qDebug() << "Broken changed:" << value;
        editingClip->broken = value;
    }
}

void ClipParamDialog::onMissingGroup_clicked( QAbstractButton *button )
{
    if ( m_bInitializing )
        return;

    bool value = button == ui->missingRadioButtonYes;
    if ( value != editingClip->missingParts )
    {
        qDebug() << "Missing changed:" << value;
        editingClip->missingParts = value;
    }
}

void ClipParamDialog::onBatteriesGroup_clicked( QAbstractButton *button )
{
    if ( m_bInitializing )
        return;

    bool value = button == ui->batRadioButtonYes;
    if ( value != editingClip->batteries )
    {
        qDebug() << "Batteries changed:" << value;
        editingClip->batteries = value;
    }
}
