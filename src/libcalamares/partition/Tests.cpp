/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2019, Adriaan de Groot <groot@kde.org>
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Tests.h"

#include "PartitionSize.h"

Q_DECLARE_METATYPE( Calamares::SizeUnit )

#include "utils/Logger.h"

#include <QtTest/QtTest>

QTEST_GUILESS_MAIN( PartitionSizeTests )

PartitionSizeTests::PartitionSizeTests()
{
}

PartitionSizeTests::~PartitionSizeTests()
{
}

void
PartitionSizeTests::initTestCase()
{
}

void 
PartitionSizeTests::testUnitComparison_data()
{
    QTest::addColumn<Calamares::SizeUnit>("u1");
    QTest::addColumn<Calamares::SizeUnit>("u2");
    QTest::addColumn<bool>("comparable");

    using Calamares::SizeUnit;
    
    QTest::newRow("nones") << SizeUnit::None << SizeUnit::None << false;
    QTest::newRow("none+%") << SizeUnit::None << SizeUnit::Percent<< false;
    QTest::newRow("%+none") << SizeUnit::Percent << SizeUnit::None << false;
    QTest::newRow("KiB+none") << SizeUnit::KiB << SizeUnit::None << false;
    QTest::newRow("none+MiB") << SizeUnit::None << SizeUnit::MiB << false;
    
    QTest::newRow("KiB+KiB") << SizeUnit::KiB << SizeUnit::KiB << true;
    QTest::newRow("KiB+MiB") << SizeUnit::KiB << SizeUnit::MiB << true;
    QTest::newRow("KiB+GiB") << SizeUnit::KiB << SizeUnit::GiB << true;
    QTest::newRow("MiB+MiB") << SizeUnit::MiB << SizeUnit::MiB << true;
    QTest::newRow("MiB+GiB") << SizeUnit::MiB << SizeUnit::GiB << true;
    QTest::newRow("GiB+GiB") << SizeUnit::GiB << SizeUnit::GiB << true;
    
    QTest::newRow("%+None") << SizeUnit::Percent << SizeUnit::None << false;
    QTest::newRow("%+%") << SizeUnit::Percent << SizeUnit::Percent << true;
    QTest::newRow("%+KiB") << SizeUnit::Percent << SizeUnit::KiB << false;
}


static bool
original_compare( Calamares::SizeUnit m_unit, Calamares::SizeUnit other_m_unit )
{
    if ( ( m_unit == Calamares::SizeUnit::None    || other_m_unit == Calamares::SizeUnit::None    ) ||
         ( m_unit == Calamares::SizeUnit::Percent && other_m_unit != Calamares::SizeUnit::Percent ) ||
         ( m_unit != Calamares::SizeUnit::Percent && other_m_unit == Calamares::SizeUnit::Percent ) )
        return false;
    return true;
}

void 
PartitionSizeTests::testUnitComparison()
{
    QFETCH( Calamares::SizeUnit, u1 );
    QFETCH( Calamares::SizeUnit, u2 );
    QFETCH( bool, comparable );
    
    if ( comparable )
    {
        QVERIFY( Calamares::PartitionSize::unitsComparable( u1, u2 ) );
        QVERIFY( Calamares::PartitionSize::unitsComparable( u2, u1 ) );
    }
    else
    {
        QVERIFY( !Calamares::PartitionSize::unitsComparable( u1, u2 ) );
        QVERIFY( !Calamares::PartitionSize::unitsComparable( u2, u1 ) );
    }
    
    QCOMPARE( original_compare( u1, u2 ), Calamares::PartitionSize::unitsComparable( u1, u2 ) );
}

void 
PartitionSizeTests::testUnitNormalisation_data()
{
    QTest::addColumn<Calamares::SizeUnit>("u1");
    QTest::addColumn<int>("v");
    QTest::addColumn<int>("bytes");
    
    using Calamares::SizeUnit;
    
    QTest::newRow("none") << SizeUnit::None << 16 << -1;
    QTest::newRow("none") << SizeUnit::None << 0 << -1;
    QTest::newRow("none") << SizeUnit::None << -2 << -1;
    
    QTest::newRow("percent") << SizeUnit::Percent << 0 << -1;
    QTest::newRow("percent") << SizeUnit::Percent << 16 << -1;
    QTest::newRow("percent") << SizeUnit::Percent << -2 << -1;
    
    QTest::newRow("KiB") << SizeUnit::KiB << 0 << 0;
    QTest::newRow("KiB") << SizeUnit::KiB << 1 << 1024;
    QTest::newRow("KiB") << SizeUnit::KiB << 1000 << 1024000;
    QTest::newRow("KiB") << SizeUnit::KiB << 1024 << 1024 * 1024;
    QTest::newRow("KiB") << SizeUnit::KiB << -2 << -1;

    QTest::newRow("MiB") << SizeUnit::MiB << 0 << 0;
    QTest::newRow("MiB") << SizeUnit::MiB << 1 << 1024 * 1024;
    QTest::newRow("MiB") << SizeUnit::MiB << 1000 << 1024 * 1024000;
    QTest::newRow("MiB") << SizeUnit::MiB << 1024 << 1024 * 1024 * 1024;
    QTest::newRow("MiB") << SizeUnit::MiB << -2 << -1;

    QTest::newRow("GiB") << SizeUnit::GiB << 0 << 0;
    QTest::newRow("GiB") << SizeUnit::GiB << 1 << 1024 * 1024 * 1024;
    QTest::newRow("GiB") << SizeUnit::GiB << 2 << 2 * 1024 * 1024 * 1024;
}

void 
PartitionSizeTests::testUnitNormalisation()
{
    QFETCH( Calamares::SizeUnit, u1 );
    QFETCH( int, v );
    QFETCH( int, bytes );
    
    QCOMPARE( Calamares::PartitionSize( v, u1 ).toBytes(), static_cast<qint64>( bytes ) );
}
