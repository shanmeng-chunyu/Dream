#include "widget_reference_book.h"
#include "ui_widget_reference_book.h"

widget_reference_book::widget_reference_book(QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_reference_book)
{
    ui->setupUi(this);

    towers=get_json_array(":/data/tower_data.json","master_towers");
    enemies=get_json_array(":/data/enemy_data.json","master_enemies");

    items={
        ui->item1,ui->item2,ui->item3,ui->item4,ui->item5,ui->item6,
        ui->item7,ui->item8,ui->item9,ui->item10,ui->item11,ui->item12};

    connect(ui->back_to_menu,&QPushButton::clicked,this,&widget_reference_book::back_to_menu);

    //切换按钮的逻辑
    connect(ui->next,&QPushButton::clicked,this,[=](){
        if(tower){
            switch_to_enemy();
            ui->upgrade->hide();
            tower=0;
            for (QAbstractButton *b : ui->buttonGroup->buttons()) {
                b->setStyleSheet("background-color: rgba(255, 255, 255,128);border-radius: 15px; "); // 恢复默认
            }
        }
        else{
            switch_to_tower();
            ui->upgrade->show();
            tower=1;
            for (QAbstractButton *b : ui->buttonGroup->buttons()) {
                b->setStyleSheet("background-color: rgba(255, 255, 255,128);border-radius: 15px; "); // 恢复默认
            }
        }
    });

    //点击按钮的逻辑
    for(int i=0;i<items.size();i++){
        connect(items[i],&QPushButton::clicked,this,[=](){
            chosen=i;
            QJsonArray &now=tower?towers:enemies;
            auto obj=now[i].toObject();
            ui->label->setPixmap(obj["pixmap"].toString());
            ui->description->setText(QString("%1\n\n%2").arg(obj["name"].toString()).arg(obj["description"].toString()));
            if(tower){
                ui->label1->setText("价 格");
                ui->label2->setText("伤 害");
                ui->value1->display(obj["cost"].toInt());
                ui->value2->display(obj["damage"].toDouble());
            }
            else{
                ui->label1->setText("生命值");
                ui->label2->setText("移 速");
                ui->value1->display(obj["health"].toInt());
                ui->value2->display(obj["speed"].toDouble());
            }
        });
    }
    //升级按钮的逻辑
    connect(ui->upgrade,&QPushButton::clicked,this,[=](){
        if(tower && chosen!=-1){
            auto obj=towers[chosen].toObject();
            ui->label->setPixmap(obj["pixmap_upgrade"].toString());
            ui->value1->display(obj["upgrade_cost"].toInt());
            ui->value2->display(obj["damage_upgrade"].toInt());
        }
    });

    //按钮组的逻辑
    connect(ui->buttonGroup,&QButtonGroup::buttonClicked,this, [=](QAbstractButton *btn){
        for (QAbstractButton *b : ui->buttonGroup->buttons()) {
            b->setStyleSheet("background-color: rgba(255, 255, 255,128);border-radius: 15px; "); // 恢复默认
        }
        // 设置当前按钮样式
        btn->setStyleSheet("background-color: rgba(255, 255, 255,128);border-radius: 15px; border: 3px solid green;");
    });

    ui->next->click();

    const QString newFontFamily = "mplus_hzk_12";

    QFont descFont = ui->description->font();
    descFont.setFamily(newFontFamily);
    ui->description->setFont(descFont);
    initialFonts[ui->description] = descFont;

    QFont label1Font = ui->label1->font();
    label1Font.setFamily(newFontFamily);
    ui->label1->setFont(label1Font);
    initialFonts[ui->label1] = label1Font;

    QFont label2Font = ui->label2->font();
    label2Font.setFamily(newFontFamily);
    ui->label2->setFont(label2Font);
    initialFonts[ui->label2] = label2Font;

    initialSize = this->size(); // 从ui文件中获取的初始尺寸

    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();
    for(int i=0;i<12;i++) initialGeometries[items[i]] = items[i]->geometry();
    initialGeometries[ui->next] = ui->next->geometry();
    initialGeometries[ui->back_to_menu] = ui->back_to_menu->geometry();
    initialGeometries[ui->upgrade] = ui->upgrade->geometry();
    initialGeometries[ui->label] = ui->label->geometry();
    initialGeometries[ui->label1] = ui->label1->geometry();
    initialGeometries[ui->label2] = ui->label2->geometry();
    initialGeometries[ui->description] = ui->description->geometry();
    initialGeometries[ui->value1] = ui->value1->geometry();
    initialGeometries[ui->value2] = ui->value2->geometry();

    // 保存图标按钮的初始图标大小
    for(int i=0;i<12;i++) initialIconSizes[items[i]] = items[i]->iconSize();
    initialIconSizes[ui->next] = ui->next->iconSize();
    initialIconSizes[ui->back_to_menu] = ui->back_to_menu->iconSize();
    initialIconSizes[ui->upgrade] = ui->upgrade->iconSize();
}

widget_reference_book::~widget_reference_book()
{
    delete ui;
}

QJsonArray widget_reference_book::get_json_array(QString path,QString name_array){
    QFile file(path);
    QJsonArray result;

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error == QJsonParseError::NoError && !doc.isNull() && doc.isObject()) {
            QJsonObject rootObj = doc.object();
            if (rootObj.contains(name_array) && rootObj[name_array].isArray()) {
                result = rootObj[name_array].toArray();
            }
        }
    }

    return result;
}

void widget_reference_book::switch_to_tower(){
    for(int i=0;i<items.size();i++){
        if(i<towers.size()){
            auto obj=towers[i].toObject();
            items[i]->setIcon(QPixmap(obj["pixmap"].toString()));
            items[i]->show();
        }
        else items[i]->hide();
    }

}

void widget_reference_book::switch_to_enemy(){
    for(int i=0;i<items.size();i++){
        if(i<enemies.size()){
            auto obj=enemies[i].toObject();
            items[i]->setIcon(QPixmap(obj["pixmap"].toString()));
            items[i]->show();
        }
        else items[i]->hide();
    }
}
