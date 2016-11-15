#!/usr/bin/perl
#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
open F_I, '<', $ARGV[0] or die "Error:$!\n";
open F_O, '>', $ARGV[1] or die "Error:$!\n";
$i_size = (stat $ARGV[0])[7];

undef $/;
$str_i=<F_I>;
(@ary_i)=unpack("C$i_size", $str_i);

if ( ($i_size % $ARGV[2]) != 0 )
{
	for ($i=0; $i<$ARGV[2] - ($i_size % $ARGV[2]); $i++)
	{
		$ary_i[$i_size + $i]=0;
	}
	$i_size=$i_size + ($ARGV[2] - ($i_size % $ARGV[2]));
}

for ($i=0; $i<$i_size; $i += $ARGV[2])
{
	for ($j=0; $j<$ARGV[2]; $j++)
	{
		$ary_o[$i+$j]=$ary_i[$i+$ARGV[2]-$j-1];
	}
}

binmode F_O;
print(F_O pack("C$i_size", @ary_o));

close F_I;
close F_O;
