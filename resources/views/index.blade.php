@extends('layouts.app')
@section('title', ($title ?? 'RinPhone') . ' | ' . config('app.name'))
@push('head.css')
    <link href="{{ mix('css/app.css') }}" rel="stylesheet" id="app-css" />
@endpush
@section('body.content')
    {!! ssr('/js/app-server.js')->fallback('<div id="app"></div>')->render() !!}
@endsection
@push('body.js')
    <script defer src="{{ mix('/js/app-client.js') }}"></script>
@endpush
